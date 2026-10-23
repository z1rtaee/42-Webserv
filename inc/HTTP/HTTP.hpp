#ifndef HTTP_HPP
# define HTTP_HPP

# include <string>
# include <cstddef>
# include <map>
# include <iostream>
# include <algorithm>
# include <cctype>
# include <cstdlib>
# include <cerrno>
# include "Status.hpp"

# define CRLF "\r\n"

typedef std::map<std::string, std::string> string_map;

enum MessageState {
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

enum ChunkState {
    CHUNK_SIZE_LINE,
    CHUNK_DATA,
    CHUNK_DATA_CRLF,
    CHUNK_TRAILERS,
    CHUNK_DONE
};

class HttpMessage {
    protected:
        string_map _headers;
        std::string _buffer;
        std::string _body;
        ParseStatus _parseStatus;
        ResponseStatus _errorStatus;

        ChunkState  _chunkState;
        std::size_t _chunkRemaining;

        static const std::size_t MAX_CHUNK_SIZE = 8388608;
        static const std::size_t MAX_BODY_SIZE = 10485760;
        static const std::size_t MAX_HEADER_SIZE = 16384;

        void parseKeyValues(std::string *line, std::string sep, string_map &out);
        void parseChunkedBody();
        void resetChunkState();
        static bool endsWithChunked(const std::string &teValue);

        static bool isTokenChar(char c);
        static bool isValidToken(const std::string &s);
        static bool isFieldVChar(unsigned char c);
        static bool isValidFieldValue(const std::string &s);
        void resetMessage();

        void setError(ResponseStatus status);

    public:
        HttpMessage();
        virtual ~HttpMessage();

        const string_map &getHeaders() const;
        const ParseStatus &getParseStatus() const;
        const ResponseStatus &getErrorStatus() const;
        void setParseStatus(const ParseStatus new_parseStatus);

        /* Shared lightweight utilities used by Request, Response and Controller. */
        static std::string trimOWS(const std::string &s);
        static std::string toLower(const std::string &s);
};

#endif