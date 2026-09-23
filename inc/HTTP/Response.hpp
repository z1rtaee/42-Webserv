#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "HTTP.hpp"
# include "Status.hpp"

class Response : public HttpMessage {
    public:
        Response();
        ~Response();
    private:
        //buildStatusLine();
        ResponseStatus _statusCode;
        std::string    _reasonPhrase;

};

#endif