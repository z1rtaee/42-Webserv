#include "HTTP/Request.hpp"

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

Request::Request() : _parseStatus(INCOMPLETE), _state(BEGIN) {
}

Request::~Request() {
}

const RequestState  &Request::getState() const {
    return _state;
}

const ParseStatus   &Request::getParseStatus() const {
    return _parseStatus;
}

void    Request::setBuffer(const std::string new_buffer) {
    _buffer = new_buffer;
}

void    Request::setState(const RequestState new_state) {
    _state = new_state;
}

void    Request::setParseStatus(const ParseStatus new_parseStatus) {
    _parseStatus = new_parseStatus;
}

bool Request::isValidMethod(const std::string &method) {
    return method == "GET" || method == "POST" || method == "DELETE";
}

bool Request::isValidVersion(const std::string &version) {
    //std::cout << version << "\n";
    return version == "HTTP/1.1";
}

bool Request::isTokenChar(char c) {
    if (std::isalnum(static_cast<unsigned char>(c))) {
        return true;
    }
    switch (c) {
        case '!': case '#': case '$': case '%': case '&': case 39: 
        case '*': case '+': case '-': case '.': case '^': case '_':
        case '`': case '|': case '~':
            return true;
        default:
            return false;
    }
}

bool Request::isValidToken(const std::string &s) {
    if (s.empty()) {
        return false; // token = 1*tchar, must be non-empty
    }
    for (std::string::size_type i = 0; i < s.size(); ++i) {
        if (!isTokenChar(s[i])) {
            return false;
        }
    }
    return true;
}

bool Request::isFieldVChar(unsigned char c) {
    return (c >= 0x21 && c <= 0x7E) || (c >= 0x80); // VCHAR or obs-text
}

std::string Request::trimOWS(const std::string &s) {
    std::string::size_type start = s.find_first_not_of(" \t");
    if (start == std::string::npos) {
        return "";
    }
    std::string::size_type end = s.find_last_not_of(" \t");
    return s.substr(start, end - start + 1);
}

bool Request::isValidFieldValue(const std::string &s) {
    for (std::string::size_type i = 0; i < s.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if (c == ' ' || c == '\t') {
            continue;
        }
        if (!isFieldVChar(c)) {
            return false;
        }
    }
    return true;
}


void Request::parseKeyValues(std::string *line, std::string sep, headers_map &out) {
    std::string name;
    std::string value;
    size_t colon;
    size_t crlf;

    if ((*line).empty()){
        setParseStatus(ERROR);
        return ;
    }
    while (!(*line).empty()) {
        colon = line->find(sep);
        //if no delim/if delim fails
        if (colon == std::string::npos || colon == 0) {
            setParseStatus(ERROR);
            break ;
        }
        /*obs-fold: a continuation line starting with SP/HTAB. Reject -
        RFC 7230 requires rejecting this unless you specifically implement
        line-unfolding support.*/
        if ((*line)[0] == ' ' || (*line)[0] == '\t') {
            std::cout << "\n" << "Rejected obs-fold / leading whitespace\n";
            setParseStatus(ERROR);
            break;
        }
        name = line->substr(0, colon);
        /*No whitespace is allowed between the header field-name and colon.
        Validating name as a strict token already covers this: SP and HTAB
        are not tchar, so a name like "Host " fails here.*/
        if (!isValidToken(name)) {
            std::cout << "\n" << "Name Failed Parsing : " << name << "\n";
            setParseStatus(ERROR); //400 bad request
            break;
        }
        /*find where THIS header line ends, so the value doesn't swallow
        the next header line too*/
        crlf = line->find(CRLF, colon + sep.length());
        if (crlf != std::string::npos)
            value = line->substr(colon + sep.length(), crlf - colon - sep.length());
        else
            value = line->substr(colon + sep.length());

        /*strip OWS: "field-name ':' OWS field-value OWS" - the leading/
        trailing space or tab around the value is not part of it*/
        value = trimOWS(value);

        if (!isValidFieldValue(value)) {
            std::cout << "\n" << "Value Failed Parsing : " << value << "\n";
            setParseStatus(ERROR); //400 bad request
            break;
        }
        /*headers are case-insensitive -> normalize it*/
        std::transform(name.begin(), name.end(), name.begin(), tolower);
        /*RFC 7230 3.2.2: multiple field-lines with the same name are
        equivalent to one field-line with the values joined by ", "*/
        headers_map::iterator it = out.find(name);
        if (it != out.end())
            it->second += ", " + value;
        else
            out[name] = value;

        std::cout << "\n" << "Name Parsed Succefully : " << name << "\n";

        /*erase this whole line (name:value pair + its CRLF) and move on
        to whatever remains in *line*/
        if (crlf != std::string::npos)
            (*line).erase(0, crlf + 2);
        else
            (*line).erase(0, (*line).size());
    }
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
    headers_map::iterator hostIt = _headers.find("host");
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

ParseStatus Request::parseRequest(const std::string request) {
    _buffer += request;

    while (!_buffer.empty()) {
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
                setParseStatus(COMPLETE);
                return getParseStatus();
        }
    }
    return getParseStatus();
}
