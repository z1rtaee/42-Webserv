#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "HTTP.hpp"
# include <sstream>

class Response : public HttpMessage {
    public:
        Response();
        ~Response();

        void setStatusCode(const ResponseStatus &new_status);
        /* For CGI/other extension codes not represented by ResponseStatus. */
        bool setStatusCode(int code, const std::string &reasonPhrase);
        const ResponseStatus &getStatusCode() const;
        int getNumericStatusCode() const;
        const std::string &getReasonPhrase() const;

        void setRequestMethod(const std::string &method);
        bool setHeader(const std::string &name, const std::string &value);
        void setBody(const std::string &body);
        std::string build() const;

    private:
        std::string buildStatusLine() const;
        std::string buildHeaderBlock() const;
        std::string capitalizeHeaderName(const std::string &lowerName) const;
        bool bodyAllowed() const;
        static void statusInfo(ResponseStatus s, int &code, std::string &reason);

        ResponseStatus _statusCode;
        int            _numericStatusCode;
        std::string    _reasonPhrase;
        std::string    _requestMethod;
};

#endif
