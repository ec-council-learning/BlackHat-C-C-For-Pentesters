#include <windows.h>
#include <iostream>
#include <array>
#include <sstream>

// Function to execute command and get output
std::string exec(const char* cmd) {
    std::array<char, 128> buffer{};
    std::string result;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

// Function to check if a given string is surrounded by quotes
bool isQuoted(const std::string& str) {
    return str.front() == '\"' && str.back() == '\"';
}

// Function to check if path points to an executable
bool isExecutable(const std::string& str) {
    return str.find(".exe") != std::string::npos;
}

// Function to check if path contains spaces
bool hasSpaces(const std::string& str) {
    return str.find(' ') != std::string::npos;
}

int main() {
    // Get all services
    std::string allServices = exec("wmic service get pathname");

    // Split into lines
    std::istringstream serviceStream(allServices);
    std::string line;

    while (std::getline(serviceStream, line)) {
        // Ignore lines with "Windows" and "Program Files" directories
        if (line.find("Windows") == std::string::npos && line.find("Program Files") == std::string::npos) {
            // Ignore lines that don't point to an executable
            if (isExecutable(line)) {
                // Remove leading and trailing whitespaces
                line.erase(0, line.find_first_not_of(" \n\r\t"));
                line.erase(line.find_last_not_of(" \n\r\t") + 1);

                // Check if it's an unquoted path with spaces
                if (!isQuoted(line) && hasSpaces(line)) {
                    std::cout << line << std::endl;
                }
            }
        }
    }

    return 0;
}
