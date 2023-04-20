#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;

int main() {
    try {
        //Define socket
        io_service io_service;
        tcp::socket socket(io_service);

        tcp::resolver resolver(io_service);
        tcp::resolver::query query("example.com", "80"); //Provide the URL (Address) and Port
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        boost::asio::connect(socket, endpoint_iterator); //Attach to socket for connection

        // Send HTTP GET request to website
        std::string request = "GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n";
        boost::asio::write(socket, boost::asio::buffer(request));

        // Read response headers
        boost::asio::streambuf response;
        boost::asio::read_until(socket, response, "\r\n");

        std::string http_version;
        unsigned int status_code;
        std::string status_message;
        std::istream response_stream(&response);
        response_stream >> http_version >> status_code;
        std::getline(response_stream, status_message);

        // Read response body
        boost::system::error_code error;
        while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error)) {
        }

        if (error != boost::asio::error::eof) {
            throw boost::system::system_error(error);
        }

        // Print response body
        std::istream response_stream2(&response);
        std::string body(std::istreambuf_iterator<char>(response_stream2), {});
        std::cout << "Response body: " << body << std::endl;

    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
