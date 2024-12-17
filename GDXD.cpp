#include <iostream>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <windows.h>
#include <stdexcept>
#include <thread>
#include <chrono>

namespace Utility {

std::string getName(const std::string& filePath) {
    try {
        std::string name = std::filesystem::path(filePath).filename().string();
        size_t ext = name.find('.');
        if (ext == std::string::npos) {
            throw std::runtime_error("Unable to extract base name from the file name.");
        }
        return name.substr(0, ext);
    } catch (const std::exception& e) {
        throw std::runtime_error("Error get name: " + std::string(e.what()));
    }
}

bool isRunning(const std::string& process) {
    std::string command = "tasklist /FI \"IMAGENAME eq " + process + ".exe\"";
    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) {
        throw std::runtime_error("Failed to open pipe for tasklist command.");
    }

    std::string output;
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;
    }
    _pclose(pipe);

    return output.find(process + ".exe") != std::string::npos;
}

int execute(const std::string& command) {
    STARTUPINFO info = { sizeof(info) };
    PROCESS_INFORMATION process;

    if (!CreateProcess(
        nullptr,
        const_cast<char*>(command.c_str()),
        nullptr,
        nullptr,
        FALSE,
        0,
        nullptr,
        nullptr,
        &info,
        &process
    )) {
        throw std::runtime_error("Failed to create process. Error code: " + std::to_string(GetLastError()));
    }

    WaitForSingleObject(process.hProcess, INFINITE);

    DWORD exitCode;
    if (!GetExitCodeProcess(process.hProcess, &exitCode)) {
        CloseHandle(process.hProcess);
        CloseHandle(process.hThread);
        throw std::runtime_error("Failed to retrieve process exit code.");
    }

    CloseHandle(process.hProcess);
    CloseHandle(process.hThread);

    return static_cast<int>(exitCode);
}

} // Utility

int main(int argc, char* argv[]) {
    try {
        if (argc < 1) {
            throw std::invalid_argument("No executable path provided.");
        }

        std::string path = argv[0];
        std::string name = Utility::getName(path);

        if (Utility::isRunning(name)) {
            std::cerr << "Error: The program is already running." << std::endl;
            return 1;
        }

        std::string exe = name + ".exe";

        if (!std::filesystem::exists(exe)) {
            throw std::runtime_error( exe + " not found.");
        }

        std::string command = exe + " -v -d";
        return Utility::execute(command);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
