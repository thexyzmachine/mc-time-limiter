#include <windows.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>

bool checkIfMinecraftOpen() {
    HWND hwnd = FindWindow(NULL, L"Minecraft");
    return hwnd != NULL;
}

void closeMc() {
    HWND hwnd = FindWindow(NULL, L"Minecraft");
    if (hwnd != NULL) {
        PostMessage(hwnd, WM_CLOSE, 0, 0);
        std::cout << "Minecraft closed.\n";
    }
}

int main() {
    int usedDay = -1;
    bool timerStarted = false;
    
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::time_t now = std::time(nullptr);
        std::tm* localTime = std::localtime(&now);
        int currentDay = localTime->tm_mday;
        
        if (currentDay != usedDay) {
            timerStarted = false;
        }
        
        if (isMinecraftOpen()) {
            if (currentDay == usedDay) {
                std::cout << "Minecraft already used today.\n";
                closeMc();
                continue;
            }
            if (!timerStarted) {
                timerStarted = true;
                std::cout << "Minecraft detected.\n";
                std::cout << "Starting 1 hour timer...\n";
                std::this_thread::sleep_for(std::chrono::hours(1));
                if (checkIfMinecraftOpen()()) {
                    closeMc();
                    usedDay = currentDay;
                    std::cout << "Time used for day "
                              << usedDay
                              << ".\n";
                }
            }
        }
    }

    return 0;
}
