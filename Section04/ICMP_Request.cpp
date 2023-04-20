// Include necessary headers
#include <iostream> // For input/output
#include <string> // For storing the hostname
#include <cstdio> // For executing the ping command
#include <memory> // For smart pointer
#include <array> // For storing the output buffer

int main()
{
    // Set the hostname to be pinged
    std::string host = "google.com";
    // Construct the ping command with the number of requests flag for Windows
    std::string ping_cmd = "ping -n 1 " + host;

    // Create a buffer to store the output of the command
    std::array<char, 128> buffer;
    std::string result;

    // Execute the ping command and read the output using a smart pointer to automatically close the pipe
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(ping_cmd.c_str(), "r"), _pclose);

    // Check if the command was executed successfully
    if (!pipe)
    {
        std::cerr << "Error: failed to execute command" << std::endl;
        return EXIT_FAILURE;
    }

    // Read the output of the command and store it in a string
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }

    // Check if the string contains the substring "Reply from" to determine if the host is up or down
    if (result.find("Reply from") != std::string::npos)
    {
        std::cout << "Host " << host << " is up" << std::endl;
    }
    else
    {
        std::cout << "Host " << host << " is down" << std::endl;
    }

    return EXIT_SUCCESS; // Exit the program with success status
}