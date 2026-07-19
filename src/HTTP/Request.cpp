#include "HTTP/Request.hpp"
#include <sstream>

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
    std::string::size_type end = line.find(CRLF);
    /*if no CRLF found*/
    if (end == std::string::npos) {
        return;
    }

    /*if found start the parsing*/
    std::string start_line = line.substr(0, end);
    std::istringstream stream(start_line);
    std::string extra;

    /*testes para ver oq acontece se parar a meio
    ve se skippa varios espaços ou so 1*/
    if (!(stream >> _method >> _target >> _version) || (stream >> extra)) {
        setParseStatus(ERROR);
        return;
    }
    if (_version != "HTTP/1.1") {
        setParseStatus(ERROR);
        return;
    }
    if (_method != "GET" && _method != "POST" && _method != "DELETE") {
        setParseStatus(ERROR);
        return;
    }
    _buffer.erase(0, _buffer.find(CRLF) + 2);
    setState(HEADER);
}

ParseStatus Request::parseRequest(const std::string &request) {
    _buffer += request;

    while (!_buffer.empty()) {
        switch (getState()) {
            case BEGIN:
                setState(START_LINE);
                continue ;
            case START_LINE:
                parseRequestLine(_buffer);
                if (getState() != HEADER)
                    return getParseStatus();
                break ;
            case HEADER:
                /*header parsing*/
                return getParseStatus();
            case EMPRY_LINE:
                /*separtes body from header*/
                return getParseStatus();
            case BODY:
                /*parse body*/
                setParseStatus(COMPLETE);
                return getParseStatus();
        }
    }
    return getParseStatus();
}
