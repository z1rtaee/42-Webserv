#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "HTTP.hpp"

class Response : public HttpMessage {
    // adds: _statusCode, _reasonPhrase, buildStatusLine()
};

#endif