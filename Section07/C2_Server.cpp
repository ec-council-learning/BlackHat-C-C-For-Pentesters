#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <map>
#include <mutex>
#include <string>

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib

std::string GetAsciiArt() {
    return R"(
_________  ________                          
\_   ___ \ \_____  \     .__         .__     
/    \  \/  /  ____/   __|  |___   __|  |___ 
\     \____/       \  /__    __/  /__    __/ 
 \______  /\_______ \    |__|        |__|    
        \/         \/                       
                                     
)";
}

std::string GetClientIPAddress(SOCKET socket) {
    sockaddr_in clientAddr;
    int addrLen = sizeof(clientAddr);
    getpeername(socket, reinterpret_cast<sockaddr*>(&clientAddr), &addrLen);
    char ipBuffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddr.sin_addr), ipBuffer, INET_ADDRSTRLEN);
    return ipBuffer;
}

void PrintHelp() {
    std::cout << "Available commands:\n";
    std::cout << "  list               List all connected clients\n";
    std::cout << "  connect <id>       Connect to a client with the specified ID\n";
    std::cout << "  kill <id>          Kill the connection with the specified ID\n";
    std::cout << "  help               Show this help message\n";
    std::cout << "  exit               Exit the server\n\n";
}

bool ReceiveResponse(SOCKET socket, std::string& response) {
    char buffer[4096];
    int bytesRead;
    response.clear();

    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(socket, &readSet);

    struct timeval timeout;
    timeout.tv_sec = 5;  // Timeout duration in seconds
    timeout.tv_usec = 0;

    // Wait for response with timeout
    int selectResult = select(socket + 1, &readSet, nullptr, nullptr, &timeout);
    if (selectResult == SOCKET_ERROR) {
        std::cerr << "select failed with error: " << WSAGetLastError() << '\n';
        return false;
    }
    else if (selectResult == 0) {
        std::cerr << "Timeout: No response received from the client.\n";
        return false;
    }

    // Receive response
    do {
        bytesRead = recv(socket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            response += buffer;
        }
    } while (bytesRead == sizeof(buffer) - 1);

    return true;
}

int main() {
    std::map<int, SOCKET> clients; // Map to store client connections
    int nextId = 1;                 // Unique ID for the next client
    std::mutex clientsMutex;        // Mutex to synchronize access to the clients map
    WSADATA wsaData;

    std::cout << GetAsciiArt();
    std::cout << "by Carson Sallis\n\n";
    PrintHelp();


    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed with error: " << result << '\n';
        return 1;
    }

    struct addrinfo* addr = nullptr;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    result = getaddrinfo(nullptr, "443", &hints, &addr); // Listen on port 443
    if (result != 0) {
        std::cerr << "getaddrinfo failed with error: " << result << '\n';
        WSACleanup();
        return 1;
    }

    SOCKET listenSocket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "socket failed with error: " << WSAGetLastError() << '\n';
        freeaddrinfo(addr);
        WSACleanup();
        return 1;
    }

    result = bind(listenSocket, addr->ai_addr, static_cast<int>(addr->ai_addrlen));
    if (result == SOCKET_ERROR) {
        std::cerr << "bind failed with error: " << WSAGetLastError() << '\n';
        freeaddrinfo(addr);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(addr);

    result = listen(listenSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        std::cerr << "listen failed with error: " << WSAGetLastError() << '\n';
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    std::thread([&clients, &clientsMutex]() {
        std::string command;
        while (true) {
            std::cout << "server> ";
            std::getline(std::cin, command);

            if (command == "list") {
                // List clients
                std::lock_guard<std::mutex> lock(clientsMutex); // Lock the mutex while accessing the clients map
                for (const auto& client : clients) {
                    int clientId = client.first;
                    SOCKET socket = client.second;
                    std::string ipAddress = GetClientIPAddress(socket);
                    std::cout << clientId << ". " << ipAddress << '\n';
                }
            }
            else if (command.substr(0, 7) == "connect") {
                // Connect to client
                int clientId = std::stoi(command.substr(8));
                std::lock_guard<std::mutex> lock(clientsMutex);
                if (clients.count(clientId) > 0) {
                    SOCKET socket = clients[clientId];
                    std::cout << "Connecting to client " << clientId << "...\n";
                    std::cout << "Connected to client " << clientId << ", type 'exit' to disconnect.\n";
                    std::string command;
                    std::string response;
                    while (true) {
                        std::cout << "client " << clientId << "> ";
                        std::getline(std::cin, command);
                        if (command == "exit") {
                            break;
                        }
                        command += "\n";
                        send(socket, command.c_str(), static_cast<int>(command.size()), 0);

                        // Receive response from the client with timeout
                        if (ReceiveResponse(socket, response)) {
                            std::cout << response;
                        }
                        else {
                            std::cout << "No response received from the client.\n";
                        }
                    }
                }
                else {
                    std::cout << "No client with ID " << clientId << '\n';
                }
            }
            else if (command.substr(0, 4) == "kill") {
                // Kill connection
                int clientId = std::stoi(command.substr(5));
                std::lock_guard<std::mutex> lock(clientsMutex);
                if (clients.count(clientId) > 0) {
                    closesocket(clients[clientId]);
                    clients.erase(clientId);
                }
                else {
                    std::cout << "No client with ID " << clientId << '\n';
                }
            }
            else if (command == "help") {
                // Display help
                PrintHelp();
            }
            else if (command == "exit") {
                // Exit the server
                std::cout << "Exiting server...\n";
                break;
            }
            else {
                std::cout << "Unknown command: " << command << '\n';
            }
        }
        }).detach();

        while (true) { // Loop to accept multiple connections
            SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
            if (clientSocket == INVALID_SOCKET) {
                std::cerr << "accept failed with error: " << WSAGetLastError() << '\n';
                closesocket(listenSocket);
                WSACleanup();
                return 1;
            }

            std::cout << "A new connection was established.\n";
            std::cout << "server>";
            std::thread([clientSocket, &clients, &nextId, &clientsMutex]() {
                int clientId;

                {
                    std::lock_guard<std::mutex> lock(clientsMutex); // Lock the mutex while modifying the clients map
                    clientId = nextId++;
                    clients[clientId] = clientSocket;
                }

                std::cout << "Accepted connection from client " << clientId << '\n';
                std::cout << "server>";
                }).detach();
        }

        // Cleanup
        closesocket(listenSocket);
        WSACleanup();

        return 0;
}
