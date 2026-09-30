#ifndef HTTP_HPP
# define HTTP_HPP

# include <string>
# include <map>
# include <iostream>
# include <algorithm>
# include <cctype>
# include <cerrno>

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
    CHUNK_SIZE_LINE,   // expecting "hex-size[;ext]\r\n"
    CHUNK_DATA,        // expecting chunk-size octets
    CHUNK_DATA_CRLF,   // expecting the CRLF that follows chunk-data
    CHUNK_TRAILERS,    // expecting trailer-part + final CRLF
    CHUNK_DONE
};

class HttpMessage {
    protected:
        string_map _headers;
        std::string _buffer;
        std::string _body;
        ParseStatus _parseStatus;

        ChunkState    _chunkState;
        std::size_t   _chunkRemaining;

        static const std::size_t MAX_CHUNK_SIZE = 8388608;   // 8 MiB per chunk
        static const std::size_t MAX_BODY_SIZE  = 10485760;  // 10 MiB total body

        void parseKeyValues(std::string *line, std::string sep, string_map &out); // shared
        void parseChunkedBody();                              // shared
        void resetChunkState();                                // for keep-alive reuse
        static bool endsWithChunked(const std::string &teValue); // shared

        static bool isTokenChar(char c);                    // shared
        static bool isValidToken(const std::string &s);        // shared
        static bool isFieldVChar(unsigned char c);           // shared
        static bool isValidFieldValue(const std::string &s);   // shared
        static std::string trimOWS(const std::string &s);       // shared
        void resetMessage();

    public:
        HttpMessage();
        virtual ~HttpMessage();
        const string_map &getHeaders() const;
        const ParseStatus &getParseStatus() const;
        void setParseStatus(const ParseStatus new_parseStatus);
};

#endif