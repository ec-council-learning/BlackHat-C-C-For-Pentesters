#include <iostream>
#include <string>
#include <boost/asio.hpp> // Boost.Asio library for networking

using boost::asio::ip::tcp; // TCP protocol for network communication

std::string make_response()
{
    std::string response_body = "<html><body>You successfully made a request to the server</body></html>"; // HTML response body
    std::string status_line = "HTTP/1.1 200 OK\r\n"; // HTTP response status line with 200 OK
    std::string content_type_header = "Content-Type: text/html\r\n"; // HTTP response content type header with HTML
    std::string content_length_header = "Content-Length: " + std::to_string(response_body.size()) + "\r\n"; // HTTP response content length header with the size of the response body
    return status_line + content_type_header + content_length_header + "\r\n" + response_body; // return the full HTTP response
}

int main()
{
    try
    {
        boost::asio::io_context io_context; // create an I/O context for Boost.Asio

        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 80)); // create a TCP acceptor object that listens on port 80

        while (true) // accept incoming connections indefinitely
        {
            tcp::socket socket(io_context); // create a TCP socket object
            acceptor.accept(socket); // accept an incoming connection and bind it to the socket

            std::cout << "Received request from: " << socket.remote_endpoint().address().to_string() << std::endl; // print the IP address of the remote endpoint

            std::string response = make_response(); // generate the HTTP response

            boost::asio::write(socket, boost::asio::buffer(response)); // send the response to the client

            socket.shutdown(tcp::socket::shutdown_both); // shutdown both read and write operations on the socket
            socket.close(); // close the socket
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl; // print any exceptions that occur
    }

    return 0; // exit the program
}
