#include "HTTP/Request.hpp"
/*
bool Request::hasWhitespace(const std::string &value) {
    for (std::string::size_type index = 0; index < value.size(); ++index) {
        const char c = value[index];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f') {
            return true;
        }
    }
    return false;
}

bool Request::isTokenChar(char c) {
    if (std::isalnum(static_cast<unsigned char>(c))) {
        return true;
    }
    switch (c) {
        case '!':
        case '#':
        case '$':
        case '%':
        case '&':
        case 39:
        case '*':
        case '+':
        case '-':
        case '.':
        case '^':
        case '_':
        case '`':
        case '|':
        case '~':
        return true;
        default:
        return false;
    }
}

*/


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

/*
bool Request::splitHeaderLine(const std::string &line, std::string &name, std::string &value) {
    
return isValidHeaderName(name);
}
*/

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
    std::cout << version << "\n";
    return version == "HTTP/1.1";
}


void Request::parseKeyValues(std::string *line, std::string sep, std::string::size_type delim) {
    const std::string::size_type colon = line->find(":");
    std::string name;
    std::string value;
    int i = 0;

    while (!(*line).empty()) {
        //if no delim/if delim fails
        if (colon == std::string::npos) {
            setParseStatus(ERROR);
            break ;
        }
        name = line->substr(i, colon);
        /*No whitespace is allowed between the header field-name and colon.*/

        /*A server MUST reject any received request message that contains
        whitespace between a header field-name and colon with a response code
        of 400 (Bad Request).*/
    }
}


void Request::parseHeaders() {
    const std::string::size_type d_crlf = _buffer.find(CRLF CRLF);

    std::cout << d_crlf << std::endl;
    if (d_crlf == std::string::npos) {
        std::cout << "Returned" << std::endl;
        return ;
    }
    std::cout << "Did Not Return" << std::endl;
    std::string header_line = _buffer.substr(0, d_crlf);
    std::cout << header_line << "\n";
    parseKeyValues(&header_line, ":", d_crlf);

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
