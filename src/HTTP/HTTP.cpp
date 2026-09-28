# include "HTTP/HTTP.hpp"

HttpMessage::HttpMessage() : _parseStatus(INCOMPLETE), _chunkState(CHUNK_SIZE_LINE), _chunkRemaining(0) {
}

HttpMessage::~HttpMessage() {
}

void HttpMessage::resetChunkState() {
    _chunkState = CHUNK_SIZE_LINE;
    _chunkRemaining = 0;
}

void HttpMessage::resetMessage() {
    _headers.clear();
    _body.clear();
    _parseStatus = INCOMPLETE;
    resetChunkState();
}

/*gzip func*/
bool HttpMessage::endsWithChunked(const std::string &teValue) {
    std::string::size_type lastComma = teValue.find_last_of(',');
    std::string lastCoding = (lastComma == std::string::npos)
        ? teValue
        : teValue.substr(lastComma + 1);

    lastCoding = trimOWS(lastCoding);

    std::string lower = lastCoding;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower == "chunked";
}

void HttpMessage::parseChunkedBody() {
    while (true) {
        switch (_chunkState) {

        case CHUNK_SIZE_LINE: {
            std::string::size_type crlf = _buffer.find(CRLF);
            if (crlf == std::string::npos) {
                if (_buffer.size() > 64) {
                    setParseStatus(ERROR); // absurdly long chunk-size line (does thi make sense?????????)
                    return;
                }
                setParseStatus(INCOMPLETE);
                return;
            }

            std::string sizeLine = _buffer.substr(0, crlf);

            // strip chunk-ext: chunk-size [ ";" chunk-ext ] - we don't
            // need extensions, just discard anything from ';' onward.
            std::string::size_type semi = sizeLine.find(';');
            std::string hexPart = (semi == std::string::npos)
                ? sizeLine
                : sizeLine.substr(0, semi);
            hexPart = trimOWS(hexPart);

            if (hexPart.empty()) {
                setParseStatus(ERROR);
                return;
            }
            for (std::string::size_type i = 0; i < hexPart.size(); ++i) {
                if (!std::isxdigit(static_cast<unsigned char>(hexPart[i]))) {
                    setParseStatus(ERROR);
                    return;
                }
            }
            if (hexPart.size() > 8) { // more than 8 hex digits -> way past MAX_CHUNK_SIZE
                setParseStatus(ERROR);
                return;
            }

            errno = 0;
            unsigned long size = std::strtoul(hexPart.c_str(), NULL, 16);
            if (errno == ERANGE || size > MAX_CHUNK_SIZE) {
                setParseStatus(ERROR);
                return;
            }
            if (_body.size() + size > MAX_BODY_SIZE) {
                setParseStatus(ERROR);
                return;
            }

            _buffer.erase(0, crlf + 2); // consume size-line + its CRLF

            if (size == 0) {
                _chunkState = CHUNK_TRAILERS; // last-chunk reached
            } else {
                _chunkRemaining = size;
                _chunkState = CHUNK_DATA;
            }
            break;
        }

        case CHUNK_DATA: {
            if (_buffer.size() < _chunkRemaining) {
                setParseStatus(INCOMPLETE); // wait for the rest of this chunk
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
                setParseStatus(ERROR);
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
            if (getParseStatus() == ERROR) {
                return; // malformed trailer header
            }

            // 7230:4.1.2: a recipient MUST ignore fields in trailers that would
            // affect message framing/routing if honored here (they were
            // already fixed by the header block) - never let a trailer
            // silently override these.
            trailers.erase("content-length");
            trailers.erase("transfer-encoding");
            trailers.erase("host");

            for (string_map::iterator it = trailers.begin(); it != trailers.end(); ++it) {
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

const string_map &HttpMessage::getHeaders() const {
	return _headers;
}

const ParseStatus &HttpMessage::getParseStatus() const {
	return _parseStatus;
}

void HttpMessage::setParseStatus(const ParseStatus new_parseStatus) {
	_parseStatus = new_parseStatus;
}

bool HttpMessage::isTokenChar(char c) {
	if (std::isalnum(static_cast<unsigned char>(c))) {
		return true;
	}
	switch (c) {
		case '!': case '#': case '$': case '%': case '&': case 39:
		case '*': case '+': case '-': case '.': case '^': case '_':
		case '`': case '|': case '~':
			return true;
		default:
			return false;
	}
}

bool HttpMessage::isValidToken(const std::string &s) {
	if (s.empty()) {
		return false;
	}
	for (std::string::size_type i = 0; i < s.size(); ++i) {
		if (!isTokenChar(s[i])) {
			return false;
		}
	}
	return true;
}

bool HttpMessage::isFieldVChar(unsigned char c) {
	return (c >= 0x21 && c <= 0x7E) || (c >= 0x80);
}

std::string HttpMessage::trimOWS(const std::string &s) {
	std::string::size_type start = s.find_first_not_of(" \t");
	if (start == std::string::npos) {
		return "";
	}
	std::string::size_type end = s.find_last_not_of(" \t");
	return s.substr(start, end - start + 1);
}

bool HttpMessage::isValidFieldValue(const std::string &s) {
	for (std::string::size_type i = 0; i < s.size(); ++i) {
		unsigned char c = static_cast<unsigned char>(s[i]);
		if (c == ' ' || c == '\t') {
			continue;
		}
		if (!isFieldVChar(c)) {
			return false;
		}
	}
	return true;
}

void HttpMessage::parseKeyValues(std::string *line, std::string sep, string_map &out) {
	std::string name;
	std::string value;
	size_t colon;
	size_t crlf;

	if ((*line).empty()){
		setParseStatus(ERROR);
		return ;
	}
	while (!(*line).empty()) {
		colon = line->find(sep);
		if (colon == std::string::npos || colon == 0) {
			setParseStatus(ERROR);
			break ;
		}
		if ((*line)[0] == ' ' || (*line)[0] == '\t') {
			std::cout << "\n" << "Rejected obs-fold / leading whitespace\n";
			setParseStatus(ERROR);
			break;
		}
		name = line->substr(0, colon);
		if (!isValidToken(name)) {
			std::cout << "\n" << "Name Failed Parsing : " << name << "\n";
			setParseStatus(ERROR);
			break;
		}
		crlf = line->find(CRLF, colon + sep.length());
		if (crlf != std::string::npos)
			value = line->substr(colon + sep.length(), crlf - colon - sep.length());
		else
			value = line->substr(colon + sep.length());
		value = trimOWS(value);
		if (!isValidFieldValue(value)) {
			std::cout << "\n" << "Value Failed Parsing : " << value << "\n";
			setParseStatus(ERROR);
			break;
		}
		std::transform(name.begin(), name.end(), name.begin(), tolower);
		string_map::iterator it = out.find(name);
		if (it != out.end())
			it->second += ", " + value;
		else
			out[name] = value;
		std::cout << "\n" << "Name Parsed Succefully : " << name << "\n";
		if (crlf != std::string::npos)
			(*line).erase(0, crlf + 2);
		else
			(*line).erase(0, (*line).size());
	}
}

