#include "HTTP/Request.hpp"
#include <cctype>

const std::map<std::string, std::string> &Request::getHeaders() const {
    return _headers;
}

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

bool Request::isValidHeaderName(const std::string &name) {
    if (name.empty()) {
        return false;
    }
    for (std::string::size_type index = 0; index < name.size(); ++index) {
        if (!isTokenChar(name[index])) {
            return false;
        }
    }
    return true;
}

std::string Request::trim(const std::string &value) {
    std::string::size_type begin = 0;
    std::string::size_type end = value.size();

    while (begin < end && (value[begin] == ' ' || value[begin] == '\t')) {
        ++begin;
    }
    while (end > begin && (value[end - 1] == ' ' || value[end - 1] == '\t')) {
        --end;
    }
    return value.substr(begin, end - begin);
}

std::string Request::toLower(const std::string &value) {
    std::string result = value;

    for (std::string::size_type index = 0; index < result.size(); ++index) {
        result[index] = static_cast<char>(std::tolower(static_cast<unsigned char>(result[index])));
    }
    return result;
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

bool Request::splitHeaderLine(const std::string &line, std::string &name, std::string &value) {
    const std::string::size_type colon = line.find(':');
    
    if (colon == std::string::npos) {
        return false;
    }

    name = line.substr(0, colon);
    value = trim(line.substr(colon + 1));

    return isValidHeaderName(name);
}

bool Request::isValidMethod(const std::string &method) {
    return method == "GET" || method == "POST" || method == "DELETE";
}

bool Request::isValidVersion(const std::string &version) {
    std::cout << version << "\n";
    return version == "HTTP/1.1";
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

void    Request::setState(const RequestState new_state) {
    _state = new_state;
}

void    Request::setParseStatus(const ParseStatus new_parseStatus) {
    _parseStatus = new_parseStatus;
}

/*
parses the start line of the HTTP request message
METHOD SP TARGET SP HTTP/version CRLF
*/
void Request::parseRequestLine(const std::string &line) {
    std::string method;
    std::string target;
    std::string version;
    const std::string::size_type crlf = line.find(CRLF);

    if (crlf == std::string::npos) {
        setParseStatus(INCOMPLETE);
        return;
    }
    if (!splitRequestLine(line.substr(0, crlf), method, target, version)) {
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

void Request::parseHeaders() {
    while (true) {
        const std::string::size_type crlf = _buffer.find(CRLF);

        if (crlf == std::string::npos) {
            setParseStatus(INCOMPLETE);
            return;
        }

        /*twin tf did i do here*/
        if (crlf == 0) {
            _buffer.erase(0, 2);
            if (_headers.find("host") == _headers.end()) {
                setParseStatus(ERROR);
                return;
            }
            setState(EMPRY_LINE);
            return;
        }

        const std::string line = _buffer.substr(0, crlf);
        std::string name;
        std::string value;

        if (!splitHeaderLine(line, name, value)) {
            setParseStatus(ERROR);
            return;
        }

        _headers[toLower(name)] = value;
        _buffer.erase(0, crlf + 2);
    }
}


/*Se erro parsing no pedido entao envio para souza a marcar como completo, COMPLETE, para ele parar de ler
*/
ParseStatus Request::parseRequest(const std::string &request) {
    _buffer += request;

    while (!_buffer.empty()) {
        switch (getState()) {
            case BEGIN:
                setState(START_LINE);
                continue ;
            case START_LINE:
                parseRequestLine(_buffer);
                if (getParseStatus() != COMPLETE || getState() != HEADER) 
                    return getParseStatus();
                break ;
            case HEADER:
                parseHeaders();
                if (getParseStatus() != COMPLETE || getState() != EMPRY_LINE)
                    return getParseStatus();
                break ;
            case EMPRY_LINE:
                setState(BODY);
                continue ;
            case BODY:
                setParseStatus(COMPLETE);
                return getParseStatus();
        }
    }
    return getParseStatus();
}
