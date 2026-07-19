#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <iostream>

# define CRLF "\r\n"

enum RequestState {
    BEGIN,
    START_LINE,
    HEADER,
    EMPRY_LINE,
    BODY
};

enum ParseStatus {
    INCOMPLETE,
    COMPLETE,
    ERROR
};

class Request {
    public:
        Request();
        ~Request();
        const RequestState  &getState() const;
        const ParseStatus   &getParseStatus() const;
        void                setState(const RequestState new_state);
        void                setParseStatus(const ParseStatus new_parseStatus);

    private:
        std::string _buffer;
        std::string _method;
        std::string _target;
        std::string _version;
        ParseStatus _parseStatus;
        RequestState _state;

        ParseStatus parseRequest(const std::string &line);
        void        parseRequestLine(const std::string &line);
};

#endif