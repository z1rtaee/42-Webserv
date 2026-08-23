#include "../inc/HTTP/Request.hpp"

/*MAIN FOR HEADERS*/
int main(void) {
    Request request;

    std::cout << "----- Should Not Return 2 CRLF -----" << std::endl;
    request.setBuffer("\r\n\r\n");
    request.parseHeaders();

    std::cout << "----- Should Not Return 2 CRLF -----" << std::endl;
    request.setBuffer("Key: Value\r\n\r\n");
    request.parseHeaders();

    std::cout << std::endl << "----- Should Return Only 1 CRLF -----" << std::endl;
    request.setBuffer("\r\n");
    request.parseHeaders();

    std::cout << std::endl << "----- Should Return No CRLF -----" << std::endl;
    request.setBuffer("Oi");
    request.parseHeaders();
}


/*MAIN FOR REQUEST LINE
int main(void) {
    Request request;
    
    request.setBuffer("GET / HTTP/1.1\r\n");
    request.parseRequestLine() ; 
    std::cout << "Should Pass" << std::endl;
    std::cout << "Parse Status: " << request.getParseStatus()<< std::endl;
    std::cout << "State: " << request.getState() << std::endl;
    request.setState(START_LINE);
    
    std::cout << "--------------------------------" << std::endl;
    
    request.setBuffer("POST / HTTP/1.1\r\n");
    request.parseRequestLine() ; 
    std::cout << "Should Pass" << std::endl;
    std::cout << "Parse Status: " << request.getParseStatus()<< std::endl;
    std::cout << "State: " << request.getState() << std::endl;
    request.setState(START_LINE);
    
    std::cout << "--------------------------------" << std::endl;

    request.setBuffer("GET /     HTTP/1.1\r\n");
    request.parseRequestLine();
    std::cout << "Should Fail" << std::endl;
    std::cout << "Parse Status: " << request.getParseStatus()<< std::endl;
    std::cout << "State: " << request.getState() << std::endl;
    request.setState(START_LINE);

    std::cout << "--------------------------------" << std::endl;

    request.setBuffer("GET / HTTP/1.2\r\n");
    request.parseRequestLine() ;
    std::cout << "Should Fail" << std::endl;
    std::cout << "Parse Status: " << request.getParseStatus()<< std::endl;
    std::cout << "State: " << request.getState() << std::endl;
    request.setState(START_LINE);
    
    std::cout << "--------------------------------" << std::endl;
    
    request.setBuffer("POSTI / HTTP/1.1\r\n");
    request.parseRequestLine() ;
    std::cout << "Should Fail" << std::endl;
    std::cout << "Parse Status: " << request.getParseStatus()<< std::endl;
    std::cout << "State: " << request.getState() << std::endl;
    request.setState(START_LINE);
    
    std::cout << "--------------------------------" << std::endl;
    
    request.setBuffer("GET HTTP/1.1 \r\n");
    request.parseRequestLine() ;
    std::cout << "Should Fail" << std::endl;
    std::cout << "Parse Status: " << request.getParseStatus()<< std::endl;
    std::cout << "State: " << request.getState() << std::endl;
    request.setState(START_LINE);
    
    return 0;
}


*/