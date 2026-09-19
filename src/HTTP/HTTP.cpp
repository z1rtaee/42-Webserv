# include "HTTP/HTTP.hpp"

HttpMessage::HttpMessage() : _parseStatus(INCOMPLETE) {
}

HttpMessage::~HttpMessage() {
}

const headers_map &HttpMessage::getHeaders() const {
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

void HttpMessage::parseKeyValues(std::string *line, std::string sep, headers_map &out) {
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
		headers_map::iterator it = out.find(name);
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

