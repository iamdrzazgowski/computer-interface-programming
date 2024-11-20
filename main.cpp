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
    dcb.BaudRate = CBR_2400;
    dcb.fParity = FALSE;
    dcb.Parity = 2;
    dcb.StopBits = ONESTOPBIT;
    dcb.ByteSize = 4;
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



    if(writeSerialPort(bufferIn, strlen(bufferIn))){
        cout << "Pomyslnie wyslano ciag znakow" << endl;
    }else {
        cout << "Blad w przeslaniu znakow" << endl;
        return false;
    }

    if(readSerialPort(bufferOut, strlen(bufferIn))){
        bufferOut[sizeof(bufferOut) - 1] = '\0';
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

    const long buffSize = 60000;

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

    while (fread(bufferIn, 1, sizeof(bufferIn), file)) {
        //cout << bufferIn;
        writeSerialPort(bufferIn, strlen(bufferIn));

        int bytesRead = readSerialPort(bufferOut, sizeof(bufferOut));
        if (bytesRead > 0) {
            bufferOut[bytesRead] = '\0';
            fwrite(bufferIn, buffSize, 1, destFile);
        }

    }

    fclose(file);
    fclose(destFile);
    CloseHandle(handle);

    return true;
}

int main() {
    //C:\Users\student\Desktop

    openSerialPort("COM3");
    dcb.DCBlength = sizeof(dcb);
    GetCommState(handle, &dcb);

    dcb.BaudRate = CBR_115200;
    dcb.fParity = FALSE;
    dcb.Parity = 2; // Upewnij się, że to jest poprawne dla twojego urządzenia
    dcb.StopBits = ONESTOPBIT;
    dcb.ByteSize = 8;
    dcb.fDtrControl = DTR_CONTROL_ENABLE;

    SetCommState(handle, &dcb);

    GetCommTimeouts(handle, &commTimeouts);
    commTimeouts.ReadTotalTimeoutConstant = 500; // Ustawienie timeoutów
    commTimeouts.WriteTotalTimeoutConstant = 500;
    SetCommTimeouts(handle, &commTimeouts);
    //serialPortConfig();
    //setTimeoutsConfig(0,0,0,0,0);

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

    return 0;
}
