#include "HTTP/HTTP.hpp"

HttpMessage::HttpMessage()
    : _parseStatus(INCOMPLETE), _errorStatus(INV),
      _chunkState(CHUNK_SIZE_LINE), _chunkRemaining(0) {}

HttpMessage::~HttpMessage() {}

void HttpMessage::setError(ResponseStatus status)
{
    _parseStatus = ERROR;
    _errorStatus = status;
}

const ResponseStatus &HttpMessage::getErrorStatus() const
{
    return _errorStatus;
}

void HttpMessage::resetChunkState()
{
    _chunkState = CHUNK_SIZE_LINE;
    _chunkRemaining = 0;
}

void HttpMessage::resetMessage()
{
    _headers.clear();
    _body.clear();
    _parseStatus = INCOMPLETE;
    _errorStatus = INV;
    resetChunkState();
}

std::string HttpMessage::toLower(const std::string &s)
{
    std::string result = s;

    for (std::string::size_type i = 0; i < result.size(); ++i) {
        result[i] = static_cast<char>(
            std::tolower(static_cast<unsigned char>(result[i])));
    }
    return result;
}

std::string HttpMessage::trimOWS(const std::string &s)
{
    std::string::size_type start = s.find_first_not_of(" \t");
    if (start == std::string::npos)
        return "";

    std::string::size_type end = s.find_last_not_of(" \t");
    return s.substr(start, end - start + 1);
}

bool HttpMessage::endsWithChunked(const std::string &teValue)
{
    std::string::size_type lastComma = teValue.find_last_of(',');
    std::string lastCoding = (lastComma == std::string::npos)
        ? teValue : teValue.substr(lastComma + 1);

    return toLower(trimOWS(lastCoding)) == "chunked";
}

void HttpMessage::parseChunkedBody()
{
    while (true) {
        switch (_chunkState) {
            case CHUNK_SIZE_LINE: {
                std::string::size_type crlf = _buffer.find(CRLF);
                if (crlf == std::string::npos) {
                    if (_buffer.size() > 64) {
                        setError(BAD_REQUEST);
                        return;
                    }
                    setParseStatus(INCOMPLETE);
                    return;
                }

                std::string sizeLine = _buffer.substr(0, crlf);
                std::string::size_type semi = sizeLine.find(';');
                std::string hexPart = (semi == std::string::npos)
                    ? sizeLine : sizeLine.substr(0, semi);

                hexPart = trimOWS(hexPart);
                if (hexPart.empty()) {
                    setError(BAD_REQUEST);
                    return;
                }
                for (std::string::size_type i = 0; i < hexPart.size(); ++i) {
                    if (!std::isxdigit(static_cast<unsigned char>(hexPart[i]))) {
                        setError(BAD_REQUEST);
                        return;
                    }
                }
                if (hexPart.size() > 8) {
                    setError(BAD_REQUEST);
                    return;
                }

                errno = 0;
                unsigned long size = std::strtoul(hexPart.c_str(), NULL, 16);
                if (errno == ERANGE || size > MAX_CHUNK_SIZE) {
                    setError(CONTENT_TOO_LARGE);
                    return;
                }
                if (_body.size() + size > MAX_BODY_SIZE) {
                    setError(CONTENT_TOO_LARGE);
                    return;
                }

                _buffer.erase(0, crlf + 2);
                if (size == 0) {
                    _chunkState = CHUNK_TRAILERS;
                } else {
                    _chunkRemaining = static_cast<std::size_t>(size);
                    _chunkState = CHUNK_DATA;
                }
                break;
            }

            case CHUNK_DATA: {
                if (_buffer.size() < _chunkRemaining) {
                    setParseStatus(INCOMPLETE);
                    return;
                }
                _body.append(_buffer, 0, _chunkRemaining);
                _buffer.erase(0, _chunkRemaining);
                _chunkRemaining = 0;
                _chunkState = CHUNK_DATA_CRLF;
                break;
            }

            case CHUNK_DATA_CRLF: {
                if (_buffer.size() < 2) {
                    setParseStatus(INCOMPLETE);
                    return;
                }
                if (_buffer.compare(0, 2, CRLF) != 0) {
                    setError(BAD_REQUEST);
                    return;
                }
                _buffer.erase(0, 2);
                _chunkState = CHUNK_SIZE_LINE;
                break;
            }

            case CHUNK_TRAILERS: {
                if (_buffer.compare(0, 2, CRLF) == 0) {
                    _buffer.erase(0, 2);
                    _chunkState = CHUNK_DONE;
                    setParseStatus(COMPLETE);
                    return;
                }

                std::string::size_type d_crlf = _buffer.find(CRLF CRLF);
                if (d_crlf == std::string::npos) {
                    setParseStatus(INCOMPLETE);
                    return;
                }

                std::string trailerBlock = _buffer.substr(0, d_crlf);
                string_map trailers;
                parseKeyValues(&trailerBlock, ":", trailers);
                if (getParseStatus() == ERROR)
                    return;

                /* Framing/routing fields are forbidden in trailers. */
                trailers.erase("content-length");
                trailers.erase("transfer-encoding");
                trailers.erase("host");
                for (string_map::iterator it = trailers.begin();
                     it != trailers.end(); ++it) {
                    _headers[it->first] = it->second;
                }

                _buffer.erase(0, d_crlf + 4);
                _chunkState = CHUNK_DONE;
                setParseStatus(COMPLETE);
                return;
            }

            case CHUNK_DONE:
                setParseStatus(COMPLETE);
                return;
        }
    }
}

const string_map &HttpMessage::getHeaders() const
{
    return _headers;
}

const ParseStatus &HttpMessage::getParseStatus() const
{
    return _parseStatus;
}

void HttpMessage::setParseStatus(const ParseStatus new_parseStatus)
{
    _parseStatus = new_parseStatus;
}

bool HttpMessage::isTokenChar(char c)
{
    if (std::isalnum(static_cast<unsigned char>(c)))
        return true;

    switch (c) {
        case '!': case '#': case '$': case '%': case '&': case '\'':
        case '*': case '+': case '-': case '.': case '^': case '_':
        case '`': case '|': case '~':
            return true;
        default:
            return false;
    }
}

bool HttpMessage::isValidToken(const std::string &s)
{
    if (s.empty())
        return false;

    for (std::string::size_type i = 0; i < s.size(); ++i) {
        if (!isTokenChar(s[i]))
            return false;
    }
    return true;
}

bool HttpMessage::isFieldVChar(unsigned char c)
{
    return (c >= 0x21 && c <= 0x7E) || c >= 0x80;
}

bool HttpMessage::isValidFieldValue(const std::string &s)
{
    for (std::string::size_type i = 0; i < s.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if (c == ' ' || c == '\t')
            continue;
        if (!isFieldVChar(c))
            return false;
    }
    return true;
}

void HttpMessage::parseKeyValues(std::string *line, std::string sep,
                                 string_map &out)
{
    std::string name;
    std::string value;
    std::string::size_type colon;
    std::string::size_type crlf;

    if (line->empty()) {
        setError(BAD_REQUEST);
        return;
    }

    while (!line->empty()) {
        colon = line->find(sep);
        if (colon == std::string::npos || colon == 0) {
            setError(BAD_REQUEST);
            break;
        }
        if ((*line)[0] == ' ' || (*line)[0] == '\t') {
            setError(BAD_REQUEST);
            break;
        }

        name = line->substr(0, colon);
        if (!isValidToken(name)) {
            setError(BAD_REQUEST);
            break;
        }

        crlf = line->find(CRLF, colon + sep.length());
        if (crlf != std::string::npos)
            value = line->substr(colon + sep.length(),
                                 crlf - colon - sep.length());
        else
            value = line->substr(colon + sep.length());

        value = trimOWS(value);
        if (!isValidFieldValue(value)) {
            setError(BAD_REQUEST);
            break;
        }

        name = toLower(name);
        string_map::iterator it = out.find(name);
        if (it != out.end())
            it->second += ", " + value;
        else
            out[name] = value;

        if (crlf != std::string::npos)
            line->erase(0, crlf + 2);
        else
            line->erase(0, line->size());
    }
}
