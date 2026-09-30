#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "HTTP.hpp"
# include "Status.hpp"
# include <sstream>

class Response : public HttpMessage {
    public:
        Response();
        ~Response();
        void                    setStatusCode(const ResponseStatus &new_status); 
        const ResponseStatus    &getStatusCode() const;
        const std::string       &getReasonPhrase() const;
        void                    setRequestMethod(const std::string &method); // needed for the HEAD body rule
        bool                    setHeader(const std::string &name, const std::string &value);
        void                    setBody(const std::string &body);

        std::string             build() const;

    private:
        std::string buildStatusLine() const;
        std::string buildHeaderBlock() const;
        std::string capitalizeHeaderName(const std::string &lowerName) const;
        bool        bodyAllowed() const;
        static void statusInfo(ResponseStatus s, int &code, std::string &reason);

        ResponseStatus _statusCode;
        std::string    _reasonPhrase;
        std::string    _requestMethod;
};

#endif
