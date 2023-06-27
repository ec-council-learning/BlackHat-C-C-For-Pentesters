#include <cpr/cpr.h>
#include <iostream>

int main() {
    cpr::Response r = cpr::Get(cpr::Url{"http://example.com"});

    std::cout << "Status code: " << r.status_code << std::endl;                  // HTTP status code
    std::cout << "Headers: " << r.header["content-type"] << std::endl;            // Header data
    std::cout << "Body: " << r.text << std::endl;                                 // Textual response
    std::cout << "Error: " << r.error.message << std::endl;                       // Error message, if any

    return 0;
}