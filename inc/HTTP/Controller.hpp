#ifndef CONTROLLER_HPP
# define CONTROLLER_HPP

# include <string>
# include <map>
# include "HTTP/Request.hpp"
# include "HTTP/Response.hpp"

/*Controller is the single boundary between the HTTP layer
and the socket layer. One Controller instance should be constructed once,
after config parsing, and reused for every client connection -
it owns no per-connection state, only server-wide configuration
such as custom error pages, so it is safe to share across clients.*/
class Controller {
    public:
        Controller();
        ~Controller();

        /*The single entry socket code calls once a
        Request has finished parsing (ParseStatus != INCOMPLETE).
        Returns the complete, ready-to-write HTTP response as raw
        bytes - status line, headers, and body all included.*/
        std::string handleRequest(Request &request) const;

        /*Registers a custom error-page body for a given status code,
        e.g. from a config directive like "error_page 404 /404.html".
        Wire this up once config parsing exposes that data; until
        then, buildErrorResponse falls back to a minimal generated
        page for any status with no registered override.*/
        void setErrorPage(ResponseStatus status, const std::string &body);

        /*Entry point for CGI output. Call this once the CGI child's
        full stdout has been collected. Parses the CGI header block per
        RFC 3875 §6, separates it from the body, and returns the complete
        ready-to-write HTTP response - same as
        handleRequest.*/
        std::string handleCGIOutput(const std::string &cgiOutput) const;

    private:
        /*Builds a response for a request that failed to parse.
        Always sends "Connection: close" per RFC 7230 §3.3.3 - once
        parsing has failed partway through a message, the byte
        stream can no longer be trusted, so the connection must not
        be reused regardless of what Connection header the client
        sent or what keepAlive() would otherwise say.*/
        Response buildErrorResponse(const Request &request) const;

        /*Produces the body for a given status: the registered custom
        page if one exists, otherwise a minimal default page built
        from the status's own reason phrase.*/
        std::string errorBody(ResponseStatus status, const std::string &reasonPhrase) const;

        /*Placeholder until the Router and method-handling layer exist. 
        Replace this function's body with real
        routing/dispatch logic; handleRequest's call site below does
        not need to change when that happens.*/
        Response route(Request &request) const;

        std::map<ResponseStatus, std::string> _errorPages;
};

#endif