#include "HTTP/Request.hpp"

Request::Request() : HttpMessage(), _state(BEGIN) {}

Request::~Request() {}

void Request::reset() {
    resetMessage();
    _method.clear();
    _target.clear();
    _version.clear();
    _state = BEGIN;
    // _buffer is  left untouched - it may already contain
    // the start of the next pipelined request
}

bool Request::keepAlive() const
{
    string_map::const_iterator it = _headers.find("connection");
    if (it == _headers.end())
        return true; /* HTTP/1.1 persistence is the default. */

    const std::string &value = it->second;
    std::string::size_type start = 0;

    while (start <= value.size()) {
        std::string::size_type comma = value.find(',', start);
        std::string token = trimOWS(value.substr(
            start, comma == std::string::npos
                ? std::string::npos : comma - start));

        /* Empty/invalid list members fail closed. */
        if (token.empty())
            return false;
        if (toLower(token) == "close")
            return false;
        if (comma == std::string::npos)
            break;
        start = comma + 1;
    }
    return true;
}

const std::string &Request::getMethod() const { return _method; }
const std::string &Request::getTarget() const { return _target; }
const std::string &Request::getVersion() const { return _version; }
const std::string &Request::getBody() const { return _body; }


bool Request::splitRequestLine(const std::string &line, std::string &method, std::string &target, std::string &version)
{
    const std::string::size_type first_space = line.find(' ');
    if (first_space == std::string::npos)
        return false;
    const std::string::size_type second_space = line.find(' ', first_space + 1);
    if (second_space == std::string::npos)
        return false;
    if (first_space == 0 || second_space == first_space + 1
        || second_space == line.size() - 1)
        return false;
    if (line.find(' ', second_space + 1) != std::string::npos)
        return false;

    method = line.substr(0, first_space);
    target = line.substr(first_space + 1, second_space - first_space - 1);
    version = line.substr(second_space + 1);
    return !method.empty() && !target.empty() && !version.empty();
}

const MessageState &Request::getState() const { return _state; }
void Request::setBuffer(const std::string new_buffer) { _buffer = new_buffer; }
void Request::setState(const MessageState new_state) { _state = new_state; }


bool Request::isValidMethod(const std::string &method) {
    return method == "GET" || method == "POST" || method == "DELETE";
}

bool Request::isValidVersion(const std::string &version) {
    //std::cout << version << "\n";
    return version == "HTTP/1.1";
}

void Request::parseHeaders()
{
    if (_buffer.compare(0, 2, CRLF) == 0) {
        _buffer.erase(0, 2);
        setError(BAD_REQUEST);
        return;
    }

    const std::string::size_type d_crlf = _buffer.find(CRLF CRLF);
    if (d_crlf == std::string::npos) {
        if (_buffer.size() > MAX_HEADER_SIZE) {
            setError(REQUEST_H_F_TOO_LARGE);
            return;
        }
        setParseStatus(INCOMPLETE);
        return;
    }
    if (d_crlf + 4 > MAX_HEADER_SIZE) {
        setError(REQUEST_H_F_TOO_LARGE);
        return;
    }

    std::string header_line = _buffer.substr(0, d_crlf);
    _buffer.erase(0, d_crlf + 4);
    parseKeyValues(&header_line, ":", _headers);
    if (getParseStatus() == ERROR)
        return;

    if (_headers.find("host") == _headers.end()) {
        setError(BAD_REQUEST);
        return;
    }
    setState(BODY);
}


/*
parses the start line of the HTTP request message
METHOD SP TARGET SP HTTP/version CRLF
*/
void Request::parseRequestLine() {
    std::string method, target, version;
    const std::string::size_type crlf = _buffer.find(CRLF);

    if (crlf == std::string::npos) {
        if (_buffer.size() > MAX_HEADER_SIZE) { // request-line itself absurdly long, no CRLF in sight
            setError(URI_TOO_LONG); // §3.1.1: 414 if the target alone is too long; using it here as
            return;                // a practical guard since we can't yet tell target from garbage
        }
        setParseStatus(INCOMPLETE);
        return;
    }
    if (!splitRequestLine(_buffer.substr(0, crlf), method, target, version)) {
        setError(BAD_REQUEST); // §3.1.1: malformed request-line -> 400
        return;
    }
    if (!isValidVersion(version)) {
        setError(HTTP_VR_NOT_SUPP); // §2.6: unsupported major/minor version -> 505
        return;
    }
    if (!isValidMethod(method)) {
        setError(NOT_IMPLEMENTED); // §3.1.1: unrecognized method -> 501
        return;
    }

    _method = method;
    _target = target;
    _version = version;
    _buffer.erase(0, crlf + 2);
    setState(HEADER);
}


void Request::parseBody()
{
    string_map::iterator te = _headers.find("transfer-encoding");
    string_map::iterator cl = _headers.find("content-length");

    if (te != _headers.end() && cl != _headers.end()) {
        setError(BAD_REQUEST);
        return;
    }
    if (te != _headers.end()) {
        if (toLower(trimOWS(te->second)) != "chunked") {
            setError(NOT_IMPLEMENTED);
            return;
        }
        parseChunkedBody();
        return;
    }
    if (cl == _headers.end()) {
        setParseStatus(COMPLETE);
        return;
    }
    if (cl->second.empty() || cl->second.size() > 19) {
        setError(BAD_REQUEST);
        return;
    }
    for (std::string::size_type i = 0; i < cl->second.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(cl->second[i]))) {
            setError(BAD_REQUEST);
            return;
        }
    }

    errno = 0;
    unsigned long parsedLength = std::strtoul(cl->second.c_str(), NULL, 10);
    if (errno == ERANGE) {
        setError(BAD_REQUEST);
        return;
    }
    if (parsedLength > MAX_BODY_SIZE) {
        setError(CONTENT_TOO_LARGE);
        return;
    }

    std::size_t len = static_cast<std::size_t>(parsedLength);
    if (_buffer.size() < len) {
        setParseStatus(INCOMPLETE);
        return;
    }

    _body = _buffer.substr(0, len);
    _buffer.erase(0, len);
    setParseStatus(COMPLETE);
}

ParseStatus Request::parseRequest(const std::string request)
{
    _buffer += request;

    while (true) {
        switch (getState()) {
            case BEGIN:
                setState(START_LINE);
                continue;
            case START_LINE:
                parseRequestLine();
                if (getState() != HEADER)
                    return getParseStatus();
                break;
            case HEADER:
                parseHeaders();
                if (getState() != BODY)
                    return getParseStatus();
                break;
            case BODY:
                parseBody();
                return getParseStatus();
        }
    }
    return getParseStatus();
}
