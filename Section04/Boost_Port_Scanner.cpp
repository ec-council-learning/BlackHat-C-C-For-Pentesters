#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h> // Required for InetPton

#pragma comment(lib, "ws2_32.lib")

int main()
{
    std::string destination_ip = "127.0.0.1"; // Hardcoded destination IP address
    unsigned short destination_port = 80; // Hardcoded destination port

    // Initialize Winsock
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
    {
        std::cerr << "Error initializing Winsock." << std::endl;
        return 1;
    }

    // Create a TCP socket
    SOCKET socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_fd == INVALID_SOCKET)
    {
        std::cerr << "Error creating socket." << std::endl;
        WSACleanup();
        return 1;
    }

    // Set the destination address and port
    sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(destination_port);

    // Convert the destination IP address to a binary address structure
    wchar_t dest_ip_wide[16];
    MultiByteToWideChar(CP_UTF8, 0, destination_ip.c_str(), -1, dest_ip_wide, 16); // Convert to wide character string
    InetPton(AF_INET, dest_ip_wide, &(dest_addr.sin_addr)); // Convert to binary address structure

    // Connect to the destination IP address and port
    if (connect(socket_fd, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) == SOCKET_ERROR)
    {
        std::cerr << "Error connecting to " << destination_ip << ":" << destination_port << std::endl;
        closesocket(socket_fd);
        WSACleanup();
        return 1;
    }

    // Print a message indicating that the port is open
    std::cout << destination_ip << ":" << destination_port << " is open." << std::endl;

    // Close the socket and clean up
    closesocket(socket_fd);
    WSACleanup();

    return 0;
}
