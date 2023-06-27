#include <windows.h> // Windows-specific headers for system functions
#include <iostream>  // For standard input/output

// The first method using the "system()" function
void method1() {
    std::cout << "Method 1: system()" << std::endl;
    system("ping -n 1 8.8.8.8"); // Execute a shell command
}

// The second method using the "WinExec()" function
void method2() {
    std::cout << "Method 2: WinExec()" << std::endl;
    // Execute a shell command, redirect output to file and hide the new console window
    WinExec("cmd /C ping -n 1 8.8.8.8 > C:\\Users\\user\\source\\repos\\Vulnerability_Demo\\x64\\Debug\\output.txt", SW_HIDE);
    system("type output.txt"); // Display the content of the output file
}

// The third method using the "ShellExecute()" function
void method3() {
    std::cout << "Method 3: ShellExecute()" << std::endl;
    // Execute a shell command, redirect output to file and hide the new console window
    ShellExecute(NULL, L"open", L"cmd.exe", L"/C ping -n 1 8.8.8.8 > C:\\Users\\user\\source\\repos\\Vulnerability_Demo\\x64\\Debug\\output.txt", NULL, SW_HIDE);
    Sleep(2000);  // Wait for 2 seconds to make sure the command finishes
    system("type output.txt"); // Display the content of the output file
}

// The fourth method using the "CreateProcess()" function
void method4() {
    std::cout << "Method 4: CreateProcess()" << std::endl;

    STARTUPINFO si;             // Structure to define the new process's main window
    PROCESS_INFORMATION pi;     // Structure to hold the new process's identification

    ZeroMemory(&si, sizeof(si));    // Initialize the structure to zero
    si.cb = sizeof(si);              // Set the size of the structure
    ZeroMemory(&pi, sizeof(pi));    // Initialize the structure to zero

    // Start the child process.
    WCHAR cmd[] = L"cmd /C ping -n 1 8.8.8.8 > output.txt"; // The command to execute
    if (!CreateProcess(NULL,   // No module name (use command line)
        cmd,        // Command line
        NULL,       // Process handle not inheritable
        NULL,       // Thread handle not inheritable
        FALSE,      // Set handle inheritance to FALSE
        0,          // No creation flags
        NULL,       // Use parent's environment block
        NULL,       // Use parent's starting directory
        &si,        // Pointer to STARTUPINFO structure
        &pi)        // Pointer to PROCESS_INFORMATION structure
        )
    {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return;
    }

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    system("type output.txt"); // Display the content of the output file
}

int main() {
    method1();  // Execute the ping command using method 1
    method2();  // Execute the ping command using method 2
    method3();  // Execute the ping command using method 3
    method4();  // Execute the ping command using method 4

    return 0;
}
