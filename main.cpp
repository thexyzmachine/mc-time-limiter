#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <thread>
#include <chrono>

bool isMinecraftOpen() {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32 process;
    process.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(snapshot, &process)) {
        do {
            if (_wcsicmp(process.szExeFile, L"javaw.exe") == 0) {
                CloseHandle(snapshot);
                return true;
            }

        } while (Process32Next(snapshot, &process));
    }

    CloseHandle(snapshot);
    return false;
}

void closeMinecraft() {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32 process;
    process.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(snapshot, &process)) {
        do {
            if (_wcsicmp(process.szExeFile, L"javaw.exe") == 0) {

                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, process.th32ProcessID);

                if (hProcess) {
                    TerminateProcess(hProcess, 0);
                    CloseHandle(hProcess);

                    std::cout << "Minecraft closed.\n";
                }
            }

        } while (Process32Next(snapshot, &process));
    }

    CloseHandle(snapshot);
}

int main() {

    while (true) {

        std::this_thread::sleep_for(std::chrono::seconds(5));

        if (isMinecraftOpen()) {

            std::cout << "Minecraft detected.\n";
            std::cout << "Waiting 1 hour...\n";

            std::this_thread::sleep_for(std::chrono::hours(1));

            // Check again before closing
            if (isMinecraftOpen()) {
                closeMinecraft();
            }
        }
    }

    return 0;
}
