#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include <windows.h>

int main()
{
    std::wstring url = L"https://www.google.com"; // Hardcoded URL
    std::wstring file_name = L"C:\\Users\\user\\Desktop\\screenshot.png"; // Hardcoded file name

    try
    {
        // Launch Chrome in headless mode
        std::wstring command = L"start chrome --enable-logging --headless --disable-gpu --screenshot --print-to-pdf=" + file_name + L" " + url;
        ShellExecuteW(NULL, L"open", L"cmd.exe", (L"/c " + command).c_str(), NULL, SW_HIDE);

        // Wait for Chrome to create the screenshot file
        Sleep(5000);

        // Rename the screenshot file
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::string old_name = converter.to_bytes(L"screenshot.png");
        std::string new_name = converter.to_bytes(file_name);
        std::rename(old_name.c_str(), new_name.c_str());

        std::wcout << L"Screenshot saved to " << file_name << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}