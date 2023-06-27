#include <iostream>
#include <vector>
#include <string>
#include <cpr/cpr.h>

std::vector<std::string> directories = { "secrets/", "backups/" };
std::vector<std::string> files = { "password.txt", "secrets.txt" };
std::string baseUrl = "http://127.0.0.1:8000/";
int maxDepth = 3;

void processDirectory(std::string directory, int currentDepth) {
    // Process files in this directory
    for (const auto& file : files) {
        std::string url = baseUrl + directory + file;
        cpr::Response r = cpr::Get(cpr::Url{ url });
        if (r.status_code == 200) {
            std::cout << "Found file: " << url << std::endl;
        }
        else {
            std::cout << "File not found: " << url << std::endl;
        }
    }

    // Don't go further if max depth is reached
    if (currentDepth >= maxDepth) {
        return;
    }

    // Process subdirectories
    for (const auto& subdirectory : directories) {
        processDirectory(directory + subdirectory, currentDepth + 1);
    }
}

int main() {
    for (const auto& directory : directories) {
        processDirectory(directory, 1);
    }

    return 0;
}
