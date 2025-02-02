#include <windows.h>
#include <setupapi.h>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib> // Do losowania
#include <ctime>   // Do inicjalizacji generatora losowego
#include <algorithm>

using namespace std;

GUID classGuid;
HMODULE hHidLib;
DWORD memberIndex = 0;
DWORD deviceInterfaceDetailDataSize;
DWORD requiredSize;
HANDLE handle;

HDEVINFO deviceInfoSet;
SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
PSP_DEVICE_INTERFACE_DETAIL_DATA deviceInterfaceDetailData = NULL;

vector<string> devicePaths;

// Rozmiar planszy
const int WIDTH = 40;
const int HEIGHT = 25;

// Bufory renderowania
char buffer1[HEIGHT][WIDTH];
char buffer2[HEIGHT][WIDTH];
char (*currentBuffer)[WIDTH] = buffer1;
char (*nextBuffer)[WIDTH] = buffer2;

// Pozycja statku
int shipX = WIDTH / 2;

int score = 0;

// Pozycje asteroid
vector<pair<int, int>> asteroids;
// Pozycje pocisków
vector<pair<int, int>> bullets;

// Funkcja do ustawienia kursora w terminalu
void setCursorPosition(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Funkcja do renderowania planszy
void render() {
    setCursorPosition(0, 0); // Ustawienie kursora na początek terminala
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            cout << currentBuffer[y][x];
        }
        cout << endl;
    }

    cout << "Score: " << score << endl;
}

// Funkcja do aktualizacji bufora
void updateBuffer() {
    // Kopiowanie bieżącego bufora do następnego
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            nextBuffer[y][x] = currentBuffer[y][x];
        }
    }

    // Czyszczenie następnego bufora
    for (int y = 1; y < HEIGHT - 1; ++y) {
        for (int x = 1; x < WIDTH - 1; ++x) {
            nextBuffer[y][x] = ' ';
        }
    }

    // Ustawienie statku
    nextBuffer[HEIGHT - 2][shipX] = '^';

    // Przesunięcie pocisków w górę
    for (auto& bullet : bullets) {
        bullet.second--;
    }

    // Usunięcie pocisków poza planszą
    bullets.erase(remove_if(bullets.begin(), bullets.end(), [](const pair<int, int>& b) {
        return b.second < 1;
    }), bullets.end());

    // Sprawdzenie kolizji pocisków z asteroidami
    for (auto it = bullets.begin(); it != bullets.end(); ) {
        bool hit = false;
        for (auto at = asteroids.begin(); at != asteroids.end(); ) {
            if (it->first == at->first && it->second == at->second) {
                at = asteroids.erase(at); // Usunięcie asteroidy
                score += 1;
                hit = true;
                break;
            } else {
                ++at;
            }
        }
        if (hit) {
            it = bullets.erase(it); // Usunięcie pocisku
        } else {
            ++it;
        }
    }

    // Sprawdzenie kolizji statku z asteroidami
    for (const auto& asteroid : asteroids) {
        if (asteroid.second == HEIGHT - 2 && asteroid.first == shipX) {
            system("cls");
            cout << "KOLIZJA! Gra zakonczona!" << endl;
            cout << "Twoj wynik wynosi: " << score << endl;
            system("pause");
            exit(0); // Kończy program
        }
    }

    // Przesunięcie asteroid w dół
    for (auto& asteroid : asteroids) {
        asteroid.second++;
    }

    // Usunięcie asteroid, które wypadły z planszy
    asteroids.erase(remove_if(asteroids.begin(), asteroids.end(),
                              [](const pair<int, int>& a) { return a.second >= HEIGHT - 1; }),
                    asteroids.end());

    // Rysowanie asteroid
    for (const auto& asteroid : asteroids) {
        if (asteroid.second < HEIGHT - 1) {
            nextBuffer[asteroid.second][asteroid.first] = '*';
        }
    }

    // Rysowanie pocisków
    for (const auto& bullet : bullets) {
        if (bullet.second >= 1) {
            nextBuffer[bullet.second][bullet.first] = '|';
        }
    }

    // Zamiana buforów
    swap(currentBuffer, nextBuffer);
}

// Funkcja do tworzenia nowych asteroid
void spawnAsteroids() {
    if (rand() % 10 < 3) { // 30% szans na pojawienie się asteroidy
        int x = rand() % (WIDTH - 2) + 1;
        asteroids.emplace_back(x, 1);
    }
}

// Funkcja do obsługi logiki gry
void gameLoop(HANDLE deviceHandle) {
    srand(time(0)); // Inicjalizacja generatora losowego

    // Inicjalizacja planszy
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            if (y == 0 || y == HEIGHT - 1) {
                currentBuffer[y][x] = '-';
            } else if (x == 0 || x == WIDTH - 1) {
                currentBuffer[y][x] = '|';
            } else {
                currentBuffer[y][x] = ' ';
            }
        }
    }

    currentBuffer[HEIGHT - 2][shipX] = '^'; // Pozycja startowa statku

    // Rozmiar bufora wejściowego
    const size_t bufferSize = 64;
    BYTE buffer[bufferSize];
    DWORD bytesRead;

    while (true) {
        // Odczyt danych z urządzenia
        if (ReadFile(deviceHandle, buffer, bufferSize, &bytesRead, NULL)) {
            // Ruch w lewo
            if ((buffer[1] == 0x00 && buffer[3] == 0x00) || (buffer[6] == 0x06)) {
                shipX = max(1, shipX - 1);
            }

            // Ruch w prawo
            if ((buffer[1] & 0xFF && buffer[3] == 0xFF) || (buffer[6] == 0x02)) {
                shipX = min(WIDTH - 2, shipX + 1);
            }

            // Atak
            if (buffer[6] == 0x4F) {
                bullets.emplace_back(shipX, HEIGHT - 3); // Dodanie pocisku powyżej statku
            }
        }

        spawnAsteroids(); // Losowe tworzenie asteroid
        updateBuffer();   // Aktualizacja planszy
        render();         // Wyświetlenie planszy
        this_thread::sleep_for(chrono::milliseconds(1)); // Spowolnienie pętli
    }
}

int main() {
    // Załaduj bibliotekę HID.dll
    hHidLib = LoadLibrary("C:\\Windows\\system32\\HID.DLL");
    if (!hHidLib) {
        cerr << "Blad dolaczenia biblioteki HID.DLL." << endl;
        return 1;
    }

    // Pobierz adres funkcji HidD_GetHidGuid
    void(__stdcall* HidD_GetHidGuid)(OUT LPGUID HidGuid);
    (FARPROC&)HidD_GetHidGuid = GetProcAddress(hHidLib, "HidD_GetHidGuid");

    if (!HidD_GetHidGuid) {
        FreeLibrary(hHidLib);
        cerr << "Nie znaleziono identyfikatora GUID." << endl;
        return 1;
    }

    // Wywołaj funkcję HidD_GetHidGuid
    HidD_GetHidGuid(&classGuid);

    // Pobierz listę urządzeń HID
    deviceInfoSet = SetupDiGetClassDevs(&classGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        cerr << "Nie zidentyfikowano podlaczonych urzadzen." << endl;
        FreeLibrary(hHidLib);
        return 1;
    }

    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    // Iteracja przez urządzenia
    while (SetupDiEnumDeviceInterfaces(deviceInfoSet, NULL, &classGuid, memberIndex, &deviceInterfaceData)) {
        memberIndex++; // Inkrementacja numeru interfejsu

        // Pobranie rozmiaru struktury szczegółów urządzenia
        SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, NULL, 0, &deviceInterfaceDetailDataSize, NULL);

        // Alokacja pamięci dla szczegółów urządzenia
        deviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA) new BYTE[deviceInterfaceDetailDataSize];
        if (!deviceInterfaceDetailData) {
            cerr << "Nie udało się alokować pamięci dla szczegółów urządzenia." << endl;
            continue;
        }
        deviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        // Pobranie szczegółów urządzenia
        if (SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, deviceInterfaceDetailData, deviceInterfaceDetailDataSize, &requiredSize, NULL)) {
            // Zapisanie ścieżki urządzenia do wektora
            devicePaths.push_back(deviceInterfaceDetailData->DevicePath);
        } else {
            cerr << "Nie mozna pobrac informacji o urzadzeniu." << endl;
        }

        // Zwolnienie pamięci
        delete[] deviceInterfaceDetailData;
        deviceInterfaceDetailData = NULL;
    }

    if (GetLastError() != ERROR_NO_MORE_ITEMS) {
        cerr << "Blad enumeracji urzadzen HID. Kod bledu: " << GetLastError() << endl;
    }

    // Wyświetlenie listy urządzeń i wybór przez użytkownika
    if (devicePaths.empty()) {
        cerr << "Nie znaleziono urzadzen HID." << endl;
    } else {
        cout << "Znalezione urządzenia HID:" << endl;
        for (size_t i = 0; i < devicePaths.size(); ++i) {
            cout << i + 1 << ". " << devicePaths[i] << endl;
        }

        int choice;
        do {
            cout << "Wybierz urzadzenie (1-" << devicePaths.size() << "): ";
            cin >> choice;
        } while (choice < 1 || choice > static_cast<int>(devicePaths.size()));

        const char* selectedDevicePath = devicePaths[choice - 1].c_str();
        cout << "Wybrano urzadzenie: " << selectedDevicePath << endl;

        // Otwórz urządzenie
        handle = CreateFile(selectedDevicePath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

        if (handle != INVALID_HANDLE_VALUE) {
            cout << "Uzyskano dostep do urzadzenia." << endl;
            system("cls");
            gameLoop(handle);
            CloseHandle(handle);
        } else {
            cerr << "Nie udalo sie uzyskac dostepu do urzadzenia. Kod bledu: " << GetLastError() << endl;
        }
    }

    // Zwolnienie zasobów
    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    FreeLibrary(hHidLib);

    return 0;
}
