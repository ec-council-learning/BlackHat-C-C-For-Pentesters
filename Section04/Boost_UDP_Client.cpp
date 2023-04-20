#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>

using boost::asio::ip::udp; // Use the UDP protocol for networking

int main()
{
    // Create an I/O context for Boost.Asio
    boost::asio::io_context io_context;

    // Create a UDP socket and bind it to port 8888
    udp::socket socket(io_context, udp::endpoint(udp::v4(), 8888));

    // Create a buffer to hold incoming data
    boost::array<char, 1024> recv_buf;

    // Create an endpoint object to hold the sender's address
    udp::endpoint remote_endpoint;

    while (true) // Listen for incoming messages indefinitely
    {
        boost::system::error_code error;
        size_t recv_len = socket.receive_from(boost::asio::buffer(recv_buf), remote_endpoint, 0, error); // Receive a message and store the sender's address in remote_endpoint

        if (error && error != boost::asio::error::message_size) // Handle any receive errors, except for message_size
        {
            std::cerr << "Error receiving data: " << error.message() << std::endl;
            continue; // Skip to the next iteration of the loop
        }

        // Convert the buffer to a string and print it to standard output
        std::string message(recv_buf.data(), recv_len);
        std::cout << "Received message: " << message << std::endl;
    }

    return 0; // Exit the program
}
