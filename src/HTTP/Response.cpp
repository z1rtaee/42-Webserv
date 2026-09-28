#include "HTTP/Response.hpp"

Response::Response() : HttpMessage(), _statusCode(INV) {}

Response::~Response() {}

const ResponseStatus &Response::getStatusCode() const {
    return _statusCode;
}

const std::string &Response::getReasonPhrase() const {
    return _reasonPhrase;
}

void Response::setStatusCode(const ResponseStatus &new_status) {
    _statusCode = new_status;
    int code;
    statusInfo(new_status, code, _reasonPhrase);
}

void Response::statusInfo(ResponseStatus s, int &code, std::string &reason) {
    switch (s) {
        // 1xx - Informational (RFC 7231 §6.2)
        case CONTINUE:              code = 100; reason = "Continue"; break;
        case SWITCHING_PROTOCOLS:   code = 101; reason = "Switching Protocols"; break;
        case PROCESSING:            code = 102; reason = "Processing"; break;          // WebDAV, RFC 2518 - out of RFC 7230 scope
        case EARLY_HINTS:           code = 103; reason = "Early Hints"; break;          // RFC 8297 - post-dates RFC 7230

        // 2xx - Success (RFC 7231 §6.3)
        case OK:                    code = 200; reason = "OK"; break;
        case CREATED:                code = 201; reason = "Created"; break;
        case ACCEPTED:               code = 202; reason = "Accepted"; break;
        case NON_AUTHORIZED_INFO:   code = 203; reason = "Non-Authoritative Information"; break;
        case NO_CONTENT:             code = 204; reason = "No Content"; break;
        case RESET_CONTENT:          code = 205; reason = "Reset Content"; break;
        case PARTIAL_CONTENT:        code = 206; reason = "Partial Content"; break;
        case MULTI_STATUS:           code = 207; reason = "Multi-Status"; break;        // WebDAV
        case ALREADY_REPORTED:       code = 208; reason = "Already Reported"; break;    // WebDAV
        case IM_USED:                code = 226; reason = "IM Used"; break;             // RFC 3229 - out of scope

        // 3xx - Redirection (RFC 7231 §6.4)
        case MULTIPLE_CHOICES:       code = 300; reason = "Multiple Choices"; break;
        case MOVED_PERMANENTLY:      code = 301; reason = "Moved Permanently"; break;
        case FOUND:                  code = 302; reason = "Found"; break;
        case SEE_OTHER:              code = 303; reason = "See Other"; break;
        case NOT_MODIFIED:           code = 304; reason = "Not Modified"; break;
        case USE_PROXY:              code = 305; reason = "Use Proxy"; break;           // deprecated per RFC 7231
        case TEMPORARY_REDIRECT:     code = 307; reason = "Temporary Redirect"; break;
        case PERMANENT_REDIRECT:     code = 308; reason = "Permanent Redirect"; break;

        // 4xx - Client Error (RFC 7231 §6.5)
        case BAD_REQUEST:                     code = 400; reason = "Bad Request"; break;
        case UNAUTHORIZED:                    code = 401; reason = "Unauthorized"; break;
        case PAYMENT_REQUIRED:                code = 402; reason = "Payment Required"; break;
        case FORBIDDEN:                       code = 403; reason = "Forbidden"; break;
        case NOT_FOUND:                       code = 404; reason = "Not Found"; break;
        case METHOD_NOT_ALLOWED:              code = 405; reason = "Method Not Allowed"; break;
        case NOT_ACCEPTABLE:                  code = 406; reason = "Not Acceptable"; break;
        case PROXY_AUTHENTICATION_REQUIRED:   code = 407; reason = "Proxy Authentication Required"; break;
        case REQUEST_TIMEOUT:                 code = 408; reason = "Request Timeout"; break;
        case CONFLICT:                        code = 409; reason = "Conflict"; break;
        case GONE:                            code = 410; reason = "Gone"; break;
        case LENGTH:                          code = 411; reason = "Length Required"; break;
        case PRECONDITION_FAILED:             code = 412; reason = "Precondition Failed"; break;
        case CONTENT_TOO_LARGE:               code = 413; reason = "Content Too Large"; break;
        case URI_TOO_LONG:                    code = 414; reason = "URI Too Long"; break;
        case UNSUPPORTED_MEDIA_TYPE:          code = 415; reason = "Unsupported Media Type"; break;
        case RANGE_NOT_SATISFIABLE:           code = 416; reason = "Range Not Satisfiable"; break;
        case EXPECTATION_FAILED:              code = 417; reason = "Expectation Failed"; break;
        case MISDIRECTED_REQUEST:             code = 421; reason = "Misdirected Request"; break;
        case UNPROCESSABLE_CONTENT:           code = 422; reason = "Unprocessable Content"; break; // WebDAV origin
        case LOCKED:                          code = 423; reason = "Locked"; break;               // WebDAV
        case FAILED_DEPENDENCY:               code = 424; reason = "Failed Dependency"; break;    // WebDAV
        case TOO_EARLY:                       code = 425; reason = "Too Early"; break;            // RFC 8470
        case UPGRADE_REQUIRED:                code = 426; reason = "Upgrade Required"; break;
        case PRECONDITION_REQUIRED:           code = 428; reason = "Precondition Required"; break;
        case TOO_MANY_REQUESTS:               code = 429; reason = "Too Many Requests"; break;
        case REQUEST_H_F_TOO_LARGE:           code = 431; reason = "Request Header Fields Too Large"; break;
        case UNAVAILABLE_FOR_LEGAL_REASONS:   code = 451; reason = "Unavailable For Legal Reasons"; break;

        // 5xx - Server Error (RFC 7231 §6.6)
        case INTERNAL_SERVER_ERROR:            code = 500; reason = "Internal Server Error"; break;
        case NOT_IMPLEMENTED:                  code = 501; reason = "Not Implemented"; break;
        case BAD_GATEWAY:                      code = 502; reason = "Bad Gateway"; break;
        case SERVICE_UNAVAILABE:               code = 503; reason = "Service Unavailable"; break;
        case GATEWAY_TIMEOUT:                  code = 504; reason = "Gateway Timeout"; break;
        case HTTP_VR_NOT_SUPP:                 code = 505; reason = "HTTP Version Not Supported"; break;
        case VARIANT_ALSO_NEGOTIATES:          code = 506; reason = "Variant Also Negotiates"; break;
        case INSUFFICIENT_STORAGE:             code = 507; reason = "Insufficient Storage"; break; // WebDAV
        case LOOP_DETECTED:                    code = 508; reason = "Loop Detected"; break;        // WebDAV
        case NOT_EXTENDED:                     code = 510; reason = "Not Extended"; break;
        case NETWORK_AUTHENTICATION_REQUIRED:  code = 511; reason = "Network Authentication Required"; break;

        case INV:
        default:
            code = 500;
            reason = "Internal Server Error";
            break;
    }
}

std::string Response::buildStatusLine() const {
    int code;
    std::string reason;

    statusInfo(_statusCode, code, reason);

    std::ostringstream oss;
    oss << "HTTP/1.1" << " " << code << " " << reason << CRLF;
    return oss.str();
}