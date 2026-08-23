#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <map>
# include <iostream>

# define CRLF "\r\n"

typedef std::map<std::string, std::string> headers_map;

enum RequestState {
    BEGIN,
    START_LINE,
    HEADER,
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
        const std::map<std::string, std::string> &getHeaders() const;
        void                setState(const RequestState new_state);
        void                setParseStatus(const ParseStatus new_parseStatus);

        void                setBuffer(const std::string new_buffer);
        void                parseRequestLine();
        void                parseHeaders();

    private:
        std::string _buffer;
        std::string _method;
        std::string _target;
        std::string _version;
        std::map<std::string, std::string> _headers;
        ParseStatus _parseStatus;
        RequestState _state;

        ParseStatus parseRequest(const std::string &line);
        //void        parseHeaders();
        void        parseKeyValues(std::string *line, std::string sep, std::string::size_type delim);
        static bool splitRequestLine(const std::string &line, std::string &method, std::string &target, std::string &version);
        static bool isValidMethod(const std::string &method);
        static bool isValidVersion(const std::string &version);
};

#endif