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

    if (file >> day) return day;
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

void warnAndCloseMinecraft() {

    HWND hwnd = FindWindowW(NULL, L"Minecraft");

    if (!hwnd) return;

    MessageBoxW(
        NULL,
        L"Please close Minecraft to save your progress.\n\n"
        L"It will close in 5 seconds if you do not exit it.",
        L"Minecraft Time Limit",
        MB_OK | MB_ICONWARNING
    );

    std::this_thread::sleep_for(std::chrono::seconds(5));

    PostMessage(hwnd, WM_CLOSE, 0, 0);

    std::cout << "Minecraft closed safely.\n";
}

int main() {

    int usedDay = loadUsedDay();
    bool timerStarted = false;

    std::cout << "Loaded usedDay: " << usedDay << "\n";

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
                std::cout << "Daily limit reached.\n";
                warnAndCloseMinecraft();
                continue;
            }

            if (!timerStarted) {

                timerStarted = true;

                std::cout << "Minecraft detected. Starting 1 hour timer...\n";

                std::this_thread::sleep_for(std::chrono::hours(1));

                if (isMinecraftOpen()) {

                    warnAndCloseMinecraft();

                    usedDay = currentDay;
                    saveUsedDay(usedDay);

                    std::cout << "Saved usedDay: " << usedDay << "\n";
                }
            }
        }
    }

    return 0;
}
