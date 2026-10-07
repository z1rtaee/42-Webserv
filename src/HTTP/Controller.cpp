#include "Controller.hpp"

Controller::Controller() {
}

Controller::~Controller() {
}

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

Response Controller::buildErrorResponse(const Request &request) const {
    Response response;
    ResponseStatus status = request.getErrorStatus();

    response.setStatusCode(status);
    response.setHeader("Connection", "close");
    response.setBody(errorBody(status, response.getReasonPhrase()));
    return response;
}

Response Controller::route(Request &request) const {
    (void)request;

    Response response;
    response.setStatusCode(OK);
    response.setBody("<html><body>Not wired up yet</body></html>");
    return response;
}

std::string Controller::handleRequest(Request &request) const {
    if (request.getParseStatus() == ERROR) {
        return buildErrorResponse(request).build();
    }
    return route(request).build();
}
