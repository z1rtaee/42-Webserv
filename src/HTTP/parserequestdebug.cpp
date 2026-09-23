#include "../../inc/HTTP/Request.hpp"

static void printResult(const std::string &label, Request &request, ParseStatus expected) {
    ParseStatus got = request.getParseStatus();

    std::cout << "----- " << label << " -----" << std::endl;
    std::cout << "Expected: "
        << (expected == COMPLETE ? "COMPLETE" : expected == INCOMPLETE ? "INCOMPLETE" : "ERROR")
        << "   Got: "
        << (got == COMPLETE ? "COMPLETE" : got == INCOMPLETE ? "INCOMPLETE" : "ERROR")
        << "   State: " << request.getState()
        << (got == expected ? "   [PASS]" : "   [FAIL]")
        << std::endl << std::endl;
}

int main(void) {
    // Case 1: a complete, well-formed, bodyless GET.
    // Every byte needed is present in a single call - nothing is
    // "still coming". This should report COMPLETE.
    {
        Request request;
        ParseStatus status = request.parseRequest("GET / HTTP/1.1\r\nHost: x\r\n\r\n");
        (void)status;
        printResult("Complete GET with Host, sent in one shot", request, COMPLETE);
    }

    // Case 2: the same request, but split across two parseRequest calls
    // the way real socket reads would arrive - to confirm the bug isn't
    // just about single-call framing.
    {
        Request request;
        request.parseRequest("GET / HTTP/1.1\r\nHo");
        std::cout << "  (after partial chunk 1) status="
            << request.getParseStatus() << " state=" << request.getState() << std::endl;
        request.parseRequest("st: x\r\n\r\n");
        printResult("Complete GET, sent in two chunks", request, COMPLETE);
    }

    // Case 3: a request still missing its terminating blank line -
    // this ONE should legitimately stay INCOMPLETE, to prove the fix
    // doesn't just rubber-stamp COMPLETE on everything.
    {
        Request request;
        request.parseRequest("GET / HTTP/1.1\r\nHost: x\r\n");
        printResult("Headers not yet terminated (missing final CRLF)", request, INCOMPLETE);
    }

    return 0;
}