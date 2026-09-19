#ifndef HTTP_HPP
# define HTTP_HPP

# define CRLF "\r\n"


class HttpMessage {
    protected:
        headers_map _headers;
        std::string _body;

        void parseKeyValues(std::string *line, std::string sep, headers_map &out); // shared
        static bool isValidToken(const std::string &s);        // shared
        static bool isValidFieldValue(const std::string &s);   // shared
        static std::string trimOWS(const std::string &s);       // shared

    public:
        const headers_map &getHeaders() const;
};

class Request : public HttpMessage {
    // adds: _method, _target, _version, parseRequestLine()
};

class Response : public HttpMessage {
    // adds: _statusCode, _reasonPhrase, buildStatusLine()
};

#endif