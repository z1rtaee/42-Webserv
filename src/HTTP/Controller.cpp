#include "Controller.hpp"

Controller::Controller() {}

Controller::~Controller() {}

void Controller::setErrorPage(ResponseStatus status, const std::string &body) {
    _errorPages[status] = body;
}

std::string Controller::errorBody(ResponseStatus status, const std::string &reasonPhrase) const {
    std::map<ResponseStatus, std::string>::const_iterator it = _errorPages.find(status);

    if (it != _errorPages.end()) {
        return it->second;
    }
    return "<html><body><h1>" + reasonPhrase + "</h1></body></html>";
}

Response Controller::buildErrorResponse(const Request &request) const
{
    Response response;
    ResponseStatus status = request.getErrorStatus();

    response.setStatusCode(status);
    response.setRequestMethod(request.getMethod());
    response.setHeader("Connection", "close");
    response.setHeader("Content-Type", "text/html; charset=utf-8");
    response.setBody(errorBody(status, response.getReasonPhrase()));
    return response;
}


Response Controller::route(Request &request) const {
    (void)request;

    Response response;
    response.setStatusCode(OK);
    response.setHeader("Content-Type", "text/html; charset=utf-8");
    response.setBody("<html><body>Not wired up yet</body></html>");
    return response;
}

std::string Controller::handleRequest(Request &request) const {
    if (request.getParseStatus() == ERROR) {
        return buildErrorResponse(request).build();
    }
    Response response = route(request);
    response.setRequestMethod(request.getMethod());
    if (!request.keepAlive())
        response.setHeader("Connection", "close");
    return response.build();
}

std::string Controller::buildCGIErrorResponse(
    ResponseStatus status, const std::string &requestMethod) const
{
    Response response;
    response.setStatusCode(status);
    response.setRequestMethod(requestMethod);
    response.setHeader("Content-Type", "text/html; charset=utf-8");
    response.setBody(errorBody(status, response.getReasonPhrase()));
    return response.build();
}


std::string Controller::handleCGIOutput(
    const std::string &cgiOutput, const std::string &requestMethod) const
{
    /* CGI output is a CGI header block, a blank line, then body bytes. */
    std::string::size_type separator = cgiOutput.find(CRLF CRLF);
    std::string::size_type separatorLength = 4;
    if (separator == std::string::npos) {
        separator = cgiOutput.find("\n\n");
        separatorLength = 2;
    }
    if (separator == std::string::npos
        || separator + separatorLength > MAX_CGI_HEADER_SIZE)
        return buildCGIErrorResponse(BAD_GATEWAY, requestMethod);

    std::string headerBlock = cgiOutput.substr(0, separator);
    std::string body = cgiOutput.substr(separator + separatorLength);
    if (headerBlock.empty())
        return buildCGIErrorResponse(BAD_GATEWAY, requestMethod);

    string_map headers;
    bool statusSeen = false;
    bool contentTypeSeen = false;
    bool locationSeen = false;
    int numericStatus = 200;
    std::string reasonPhrase = "OK";
    std::string locationValue;

    std::istringstream input(headerBlock);
    std::string line;
    while (std::getline(input, line)) {
        /* getline consumes LF; accept CGI's CRLF by removing its CR. */
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        /* CGI/1.1 does not permit folded header lines. */
        if (line.empty() || line[0] == ' ' || line[0] == '\t')
            return buildCGIErrorResponse(BAD_GATEWAY, requestMethod);

        std::string::size_type colon = line.find(':');
        if (colon == std::string::npos || colon == 0)
            return buildCGIErrorResponse(BAD_GATEWAY, requestMethod);

        std::string name = line.substr(0, colon);
        std::string value = HttpMessage::trimOWS(line.substr(colon + 1));

        /* Response already owns the shared HTTP token/value validators. */
        Response validator;
        if (!validator.setHeader(name, value))
            return buildCGIErrorResponse(BAD_GATEWAY, requestMethod);
        name = HttpMessage::toLower(name);

        if (name == "status") {
            if (statusSeen || !parseCGIStatus(value, numericStatus, reasonPhrase))
                return buildCGIErrorResponse(BAD_GATEWAY, requestMethod);
            statusSeen = true;
            continue;
        }
        if (name == "content-type") {
            if (contentTypeSeen || value.empty())
                return buildCGIErrorResponse(BAD_GATEWAY, requestMethod);
            contentTypeSeen = true;
        }
        if (name == "location") {
            if (locationSeen || value.empty())
                return buildCGIErrorResponse(BAD_GATEWAY, requestMethod);
            locationSeen = true;
            locationValue = value;
        }

        string_map::iterator existing = headers.find(name);
        if (existing != headers.end()) {
            /* This map cannot represent multiple Set-Cookie fields safely. */
            if (name == "set-cookie" || name == "content-type" || name == "location")
                return buildCGIErrorResponse(BAD_GATEWAY, requestMethod);
            existing->second += ", " + value;
        } else {
            headers[name] = value;
        }
    }

    /* A valid CGI response is one of document, local redirect or client redirect. */
    bool simpleClientRedirect = false;
    bool localRedirect = false;
    if (locationSeen && !statusSeen) {
        if (!body.empty() || headers.size() != 1 || contentTypeSeen)
            return buildCGIErrorResponse(BAD_GATEWAY, requestMethod);

        if (isAbsoluteCGIURI(locationValue)) {
            simpleClientRedirect = true;
            numericStatus = 302;
            reasonPhrase = "Found";
        } else if (!locationValue.empty() && locationValue[0] == '/') {
            localRedirect = true;
        } else {
            return buildCGIErrorResponse(BAD_GATEWAY, requestMethod);
        }
    } else if (locationSeen) {
        /* A redirect with a document requires absolute Location, Status and Content-Type. */
        if (!isAbsoluteCGIURI(locationValue) || !contentTypeSeen
            || !isClientRedirectStatus(numericStatus))
            return buildCGIErrorResponse(BAD_GATEWAY, requestMethod);
    } else if (!contentTypeSeen) {
        return buildCGIErrorResponse(BAD_GATEWAY, requestMethod);
    }

    /* Local redirects require internal re-dispatch, not a client HTTP response.
       This API has no Request/Router result channel, so make the limitation clear. */
    if (localRedirect)
        return buildCGIErrorResponse(NOT_IMPLEMENTED, requestMethod);

    std::map<std::string, bool> connectionOptions;
    string_map::const_iterator connection = headers.find("connection");
    if (connection != headers.end()
        && !parseConnectionOptions(connection->second, connectionOptions))
        return buildCGIErrorResponse(BAD_GATEWAY, requestMethod);

    /* Required CGI fields must not be removed by Connection options. */
    if (connectionOptions.find("content-type") != connectionOptions.end()
        || connectionOptions.find("location") != connectionOptions.end())
        return buildCGIErrorResponse(BAD_GATEWAY, requestMethod);

    Response response;
    response.setRequestMethod(requestMethod);

    if (simpleClientRedirect) {
        response.setStatusCode(FOUND);
    } else if (statusSeen) {
        if (!response.setStatusCode(numericStatus, reasonPhrase))
            return buildCGIErrorResponse(BAD_GATEWAY, requestMethod);
    } else {
        response.setStatusCode(OK);
    }

    for (string_map::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        const std::string &name = it->first;
        if (name == "content-length" || name == "transfer-encoding"
            || isHopByHopHeader(name)
            || connectionOptions.find(name) != connectionOptions.end())
            continue;
        if (!response.setHeader(name, it->second))
            return buildCGIErrorResponse(BAD_GATEWAY, requestMethod);
    }

    /* HTTP forbids content on 204 and 304. 205 needs an explicit zero length
       in HTTP/1.1 when the connection is to remain persistent. */
    if (numericStatus == 204 || numericStatus == 304) {
        /* Deliberately no Content-Length on these status codes. */
    } else if (numericStatus == 205) {
        response.setBody("");
    } else {
        /* Also sets the right representation length for HEAD; build() omits body. */
        response.setBody(body);
    }

    return response.build();
}
