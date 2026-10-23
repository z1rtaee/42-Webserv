#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "HTTP.hpp"

class Request : public HttpMessage {
    public:
        Request();
        ~Request();

        const MessageState  &getState() const;
        void                setState(const MessageState new_state);
        const std::string   &getMethod() const;
        const std::string   &getTarget() const;
        const std::string   &getVersion() const;
        const std::string   &getBody() const;

        void                setBuffer(const std::string new_buffer);
        void                parseRequestLine(); /* private in final design */
        void                parseHeaders();     /* private in final design */
        void                parseBody();       /* private in final design */
        ParseStatus         parseRequest(const std::string request); /* private in final design */

        void                reset();
        bool                keepAlive() const;

    private:
        std::string _method;
        std::string _target;
        std::string _version;
        MessageState _state;

        static bool splitRequestLine(const std::string &line,
                                     std::string &method,
                                     std::string &target,
                                     std::string &version);
        static bool isValidMethod(const std::string &method);
        static bool isValidVersion(const std::string &version);
};

#endif
