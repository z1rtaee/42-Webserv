#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "HTTP.hpp"

enum RequestState {
    BEGIN,
    START_LINE,
    HEADER,
    BODY
};

class Request : public HttpMessage {
    public:
        Request();
        ~Request();
        const RequestState  &getState() const;
        void                setState(const RequestState new_state);

        void                setBuffer(const std::string new_buffer);
        void                parseRequestLine(); /*private*/
        void                parseHeaders();/*private*/
        ParseStatus         parseRequest(const std::string line); /*private*/

    private:
        std::string _buffer;
        std::string _method;
        std::string _target;
        std::string _version;
        RequestState _state;

        static bool splitRequestLine(const std::string &line, std::string &method, std::string &target, std::string &version);
        static bool isValidMethod(const std::string &method);
        static bool isValidVersion(const std::string &version);
};

#endif