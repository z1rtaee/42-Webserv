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

bool Request::splitRequestLine(const std::string &line, std::string &method, std::string &target, std::string &version) {
    const std::string::size_type first_space = line.find(' ');
    const std::string::size_type second_space = line.find(' ', first_space + 1);
    
    if (first_space == std::string::npos || second_space == std::string::npos) {
        return false;
    }
    if (first_space == 0 || second_space == first_space + 1 || second_space == line.size() - 1) {
        return false;
    }
    if (line.find(' ', second_space + 1) != std::string::npos) {
        return false;
    }
    
    method = line.substr(0, first_space);
    target = line.substr(first_space + 1, second_space - first_space - 1);
    version = line.substr(second_space + 1);
    
    if (method.empty() || target.empty() || version.empty()) {
        return false;
    }
    return true;
}

const MessageState  &Request::getState() const {
    return _state;
}

void    Request::setBuffer(const std::string new_buffer) {
    _buffer = new_buffer;
}

void    Request::setState(const MessageState new_state) {
    _state = new_state;
}

bool Request::isValidMethod(const std::string &method) {
    return method == "GET" || method == "POST" || method == "DELETE";
}

bool Request::isValidVersion(const std::string &version) {
    //std::cout << version << "\n";
    return version == "HTTP/1.1";
}

void Request::parseHeaders() {
    const std::string::size_type d_crlf = _buffer.find(CRLF CRLF);

    std::cout << "DOUBLE CRLF INDEX: " << d_crlf << std::endl;
    if (_buffer.compare(0, 2, CRLF) == 0) {
        _buffer.erase(0, 2);
        setParseStatus(ERROR);
        return;
    }
    if (d_crlf == std::string::npos) {
        std::cout << "Returned" << std::endl;
        setParseStatus(INCOMPLETE);
        return ;
    }
    std::cout << "Did Not Return" << std::endl;
    std::string header_line = _buffer.substr(0, d_crlf);
    std::cout << header_line << "\n";
    /*clean all header and crl crlf (4 bytes) will be ready for next move*/
    _buffer.erase(0, d_crlf + 4); 
    parseKeyValues(&header_line, ":", _headers);
    if (getParseStatus() == ERROR)
        return;
    string_map::iterator hostIt = _headers.find("host");
    if (hostIt == _headers.end()) {
        setParseStatus(ERROR); // 400 - Host required in HTTP/1.1
        return;
    }
    setState(BODY);
}


/*
parses the start line of the HTTP request message
METHOD SP TARGET SP HTTP/version CRLF
*/
void Request::parseRequestLine() {
    std::string method;
    std::string target;
    std::string version;
    const std::string::size_type crlf = _buffer.find(CRLF);

    if (crlf == std::string::npos) {
        setParseStatus(INCOMPLETE);
        return;
    }
    if (!splitRequestLine(_buffer.substr(0, crlf), method, target, version)) {
        std::cout << "Splitting Failed" << std::endl;
        setParseStatus(ERROR);
        return;
    }
    
    if (!isValidVersion(version)) {
        std::cout << "Inv Version of HTTP" << std::endl;
        std::cout << "purrrrrrrrrrrrrrr";
        setParseStatus(ERROR);
        return;
    }
    
    if (!isValidMethod(method)) {
        std::cout << "Inv Method of HTTP" << std::endl;
        setParseStatus(ERROR);
        return;
    }

    _method = method;
    _target = target;
    _version = version;
    _buffer.erase(0, crlf + 2);
    setState(HEADER);
    std::cout << "Successful" << std::endl;
}

void Request::parseBody() {
    string_map::iterator te = _headers.find("transfer-encoding");
    string_map::iterator cl = _headers.find("content-length");

    if (te != _headers.end() && cl != _headers.end()) {
        setParseStatus(ERROR); // 9112/7230:3.3.3: both present -> reject //check if it is like this or add with virgula
        return;
    }
    if (te != _headers.end()) {
        /*only chunked no gzip*/
        std::string normalized = te->second;
        std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
        if (normalized != "chunked") {
            setParseStatus(ERROR); //no gzip/deflate/compress
            return;
        }
        /*only chunk if support gzip take this out and check if ends with chunked func above*/
        parseChunkedBody();
        return;
    }
    if (cl == _headers.end()) {
        setParseStatus(COMPLETE); // no framing header -> no body loool
        return;
    }
    if (cl->second.empty() || cl->second.size() > 19) {
        setParseStatus(ERROR);
        return;
    }
    for (std::string::size_type i = 0; i < cl->second.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(cl->second[i]))) {
            setParseStatus(ERROR);
            return;
        }
    }
    errno = 0;
    std::size_t len = std::strtoul(cl->second.c_str(), NULL, 10);
    if (errno == ERANGE) {
        setParseStatus(ERROR);
        return;
    }
    if (_buffer.size() < len) {
        setParseStatus(INCOMPLETE);
        return;
    }
    _body = _buffer.substr(0, len);
    _buffer.erase(0, len);
    setParseStatus(COMPLETE);
}

ParseStatus Request::parseRequest(const std::string request) {
    _buffer += request;

    while (true) {
        switch (getState()) {
            case BEGIN:
                setState(START_LINE);
                continue ;
            case START_LINE:
                parseRequestLine();
                if (getState() != HEADER) 
                    return getParseStatus();
                break ;
            case HEADER:
                parseHeaders();
                if (getState() != BODY)
                    return getParseStatus();
                break ;
            
            case BODY:
                parseBody();
                return getParseStatus();
        }
    }
    return getParseStatus();
}
