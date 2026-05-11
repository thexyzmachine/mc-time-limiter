#include <windows.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <fstream>
#include <string>

const std::string FILE_NAME = "mc_data.txt";

int loadUsedDay() {
    std::ifstream file(FILE_NAME);
    int day;

    if (file >> day) {
        return day;
    }

    return -1; 
}

void saveUsedDay(int day) {
    std::ofstream file(FILE_NAME);
    file << day;
}

bool isMinecraftOpen() {
    bool found = false;

    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {

        wchar_t title[256];
        GetWindowTextW(hwnd, title, 256);

        std::wstring windowTitle = title;

        if (windowTitle.find(L"Minecraft") != std::wstring::npos) {
            *(bool*)lParam = true;
            return FALSE;
        }

        return TRUE;

    }, (LPARAM)&found);

    return found;
}

void closeMc() {
    EnumWindows([](HWND hwnd, LPARAM) -> BOOL {

        wchar_t title[256];
        GetWindowTextW(hwnd, title, 256);

        std::wstring windowTitle = title;

        if (windowTitle.find(L"Minecraft") != std::wstring::npos) {
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            std::cout << "Minecraft closed.\n";
            return FALSE;
        }

        return TRUE;

    }, 0);
}

int main() {

    int usedDay = loadUsedDay();
    bool timerStarted = false;

    std::cout << "Loaded usedDay = " << usedDay << "\n";

    while (true) {

        std::this_thread::sleep_for(std::chrono::seconds(5));

        std::time_t now = std::time(nullptr);
        std::tm* local = std::localtime(&now);
        int currentDay = local->tm_mday;

        if (currentDay != usedDay) {
            timerStarted = false;
        }

        if (isMinecraftOpen()) {

            if (usedDay == currentDay) {
                std::cout << "Limit already used today.\n";
                closeMc();
                continue;
            }

            if (!timerStarted) {

                timerStarted = true;

                std::cout << "Minecraft detected. Starting 1 hour timer...\n";

                std::this_thread::sleep_for(std::chrono::hours(1));

                if (isMinecraftOpen()) {

                    closeMc();

                    usedDay = currentDay;
                    saveUsedDay(usedDay);

                    std::cout << "Saved usedDay = " << usedDay << "\n";
                }
            }
        }
    }

    return 0;
}
