#ifndef HTTP_HPP
# define HTTP_HPP

# include <string>
# include <map>
# include <iostream>
# include <algorithm>
# include <cctype>

# define CRLF "\r\n"

typedef std::map<std::string, std::string> headers_map;

enum ParseStatus {
    INCOMPLETE,
    COMPLETE,
    ERROR
};

class HttpMessage {
    protected:
        headers_map _headers;
        std::string _body;
        ParseStatus _parseStatus;

        void parseKeyValues(std::string *line, std::string sep, headers_map &out); // shared
        static bool isTokenChar(char c);                    // shared
        static bool isValidToken(const std::string &s);        // shared
        static bool isFieldVChar(unsigned char c);           // shared
        static bool isValidFieldValue(const std::string &s);   // shared
        static std::string trimOWS(const std::string &s);       // shared

    public:
        HttpMessage();
        virtual ~HttpMessage();
        const headers_map &getHeaders() const;
        const ParseStatus &getParseStatus() const;
        void setParseStatus(const ParseStatus new_parseStatus);
};

#endif