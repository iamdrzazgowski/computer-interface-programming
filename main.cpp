#include <iostream>
#include <windows.h>
#include <string>
#include <cstdio>

using namespace std;

HANDLE handle;
DCB dcb;
COMMTIMEOUTS commTimeouts;

int writeSerialPort(void *buffer, unsigned long numberOfBytesToWrite) {
    unsigned long numberOfBytesWritten = 0;
    bool result = WriteFile(handle, buffer, numberOfBytesToWrite, &numberOfBytesWritten, NULL);

    if (!result) {
        cout << "Blad przeslania danych" << endl;
        return 0;
    }
    return numberOfBytesWritten;
}

int readSerialPort(void *buffer, unsigned long numberOfBytesToRead) {
    unsigned long bytesRead = 0;
    bool result = ReadFile(handle, buffer, numberOfBytesToRead, &bytesRead, NULL);

    if (!result) {
        cout << "Blad odczytu danych" << endl;
        return 0;
    }
    return bytesRead;
}

bool openSerialPort(const char * port){
    handle = CreateFile(port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if(handle == INVALID_HANDLE_VALUE){
        cout << "Blad otwarcia portu" << endl;
        return false;
    }

    return true;
}

bool serialPortConfig(){

    if(GetCommState(handle, &dcb) == 0){
        cout << "Problem z ustawieniem parametrow portu" << endl;
        CloseHandle(handle);
        return false;
    }

    dcb.DCBlength = sizeof(dcb);
    dcb.BaudRate = CBR_256000;
    dcb.fParity = FALSE;
    dcb.Parity = 2;
    dcb.StopBits = ONESTOPBIT;
    dcb.ByteSize = 8;
    dcb.fDtrControl = DTR_CONTROL_ENABLE;

    SetCommState(handle, &dcb);

    return true;
}

bool setTimeoutsConfig(unsigned long ReadIntervalTimeout, unsigned long ReadTotalTimeoutMultiplier, unsigned long ReadTotalTimeoutConstant, unsigned long WriteTotalTimeoutMultiplier, unsigned long WriteTotalTimeoutConstant){

    if(GetCommTimeouts (handle, &commTimeouts)==0) return false;

    commTimeouts.ReadIntervalTimeout = ReadIntervalTimeout;
    commTimeouts.ReadTotalTimeoutConstant = ReadTotalTimeoutConstant;
    commTimeouts.ReadTotalTimeoutMultiplier = ReadTotalTimeoutMultiplier;
    commTimeouts.WriteTotalTimeoutConstant = WriteTotalTimeoutConstant;
    commTimeouts.WriteTotalTimeoutMultiplier = WriteTotalTimeoutMultiplier;

    if(SetCommTimeouts(handle, &commTimeouts) == 0){
        cout << "Błąd ustawienia parametrow Timeouts";
        CloseHandle(handle);
        return false;
    }

    return true;
}


bool sendString(){
    char bufferIn[1024] = {0};
    char bufferOut[1024] = {0};

    cout << "Podaj tekst do wyslania: ";
    cin.ignore();
    cin.getline(bufferIn, sizeof(bufferIn));


    if(writeSerialPort(bufferIn, strlen(bufferIn))){
        cout << "Rozpoczeto przesywanie ciagu znakow..." << endl;
    }else {
        cout << "Blad w przeslaniu znakow" << endl;
        return false;
    }

    if(readSerialPort(bufferOut, strlen(bufferIn))){
        bufferOut[sizeof(bufferOut) - 1] = '\0';
        cout << "Zakonczono odczyt ciagu znakow." << endl;
        cout << "Odczytany tekst: " << bufferOut << endl;
    }else {
        cout << "Blad w odczytaniu tekstu" << endl;
        return false;
    }

    return true;
}

bool sendFile(){

    FILE * file;
    FILE * destFile;

    const long buffSize = 2048;

    char bufferIn[buffSize] = {0};
    char bufferOut[buffSize] = {0};

    char path[128] = {0};
    char destPath[128] = {0};

    cout << "Podaj sciezke do pliku: ";
    cin >> path;

    cout << "Podaj gdzie ma zostac zapisany nowy plik: ";
    cin >> destPath;

    file = fopen(path, "rb");
    if (file == NULL) {
        cout << "Nie mozna otworzc pliku wejsciowego." << endl;
        return false;
    }

    destFile = fopen(destPath, "ab");

    if (destFile == NULL) {
        cout << "Nie mozna otworzc pliku wyjsciowego." << endl;
        return false;
    }

    cout << "Rozpoczeto przesylanie pliku..." << endl;
    fseek(file,0,SEEK_END);
    long totalSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    long currentSize = 0;

//    cout << "Rozmiar pliku: " << size << endl;

    while(currentSize <= totalSize){
        long bytesToRead = min(buffSize, totalSize - currentSize);
        fread(bufferIn, 1,bytesToRead, file);
        writeSerialPort(bufferIn, bytesToRead);

        // Odbiór danych
        if (readSerialPort(bufferOut, bytesToRead) == 0) {
            break;
        }

        // Zapis do pliku docelowego
        fwrite(bufferOut, 1,bytesToRead, destFile);
        currentSize += bytesToRead;
        cout << "Przeslano: " << currentSize << " z " << totalSize << " bajtow." << endl;
    }

    cout << "Zakonczono przesylanie pliku." << endl;

    fclose(file);
    fclose(destFile);
    CloseHandle(handle);

    return true;
}

int main() {

    char port[64] = {0};
    cout << "Podaj port: ";
    cin >> port;

    if(openSerialPort(port)){
        GetCommState(handle, &dcb);

        serialPortConfig();

        SetCommState(handle, &dcb);

        GetCommTimeouts(handle, &commTimeouts);
        setTimeoutsConfig(0,0,1000,0,0);
        SetCommTimeouts(handle, &commTimeouts);

        cout << "Wybierz opcje: " << endl;
        cout << "1. Przeslanie ciagu znakow" << endl;
        cout << "2. Przeslanie pliku" << endl;

        int option;
        cout << "Wybierz opcje: ";
        cin >> option;
        cout << "" << endl;

        if(option == 1){
            sendString();
        }else if(option == 2){
            sendFile();
        }else {
            cout << "Bledna opcja." << endl;
        }
    };

//    GetCommState(handle, &dcb);
//
//    serialPortConfig();
//
//    SetCommState(handle, &dcb);
//
//    GetCommTimeouts(handle, &commTimeouts);
//    setTimeoutsConfig(0,0,1000,0,0);
//    SetCommTimeouts(handle, &commTimeouts);
//
//    cout << "Wybierz opcje: " << endl;
//    cout << "1. Przeslanie ciagu znakow" << endl;
//    cout << "2. Przeslanie pliku" << endl;
//
//    int option;
//    cout << "Wybierz opcje: ";
//    cin >> option;
//    cout << "" << endl;
//
//    if(option == 1){
//        sendString();
//    }else if(option == 2){
//        sendFile();
//    }else {
//        cout << "Bledna opcja." << endl;
//    }

    std::cout << "Nacisnij Enter, aby zamknac program..." << std::endl;
    std::cin.get();

    return 0;
}
