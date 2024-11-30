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

void printSerialPortConfig(){

    GetCommState(handle, &dcb);

    cout << "Konfiguracja portu: " << endl;
    cout << "BaudRate: " << dcb.BaudRate << endl;
    cout << "Parity: " << dcb.Parity << endl;
    cout << "StopBits: " << dcb.StopBits << endl;
    cout << "ByteSize: " << int(dcb.ByteSize) << endl;
}

bool serialPortConfig(){

    if(GetCommState(handle, &dcb) == 0){
        cout << "Problem z ustawieniem parametrow portu" << endl;
        CloseHandle(handle);
        return false;
    }

    dcb.DCBlength = sizeof(dcb);
//    dcb.BaudRate = CBR_256000;
    dcb.fParity = FALSE;
//    dcb.Parity = 2;
//    dcb.StopBits = ONESTOPBIT;
//    dcb.ByteSize = 8;
    dcb.fDtrControl = DTR_CONTROL_ENABLE;

    int  baudRateOption = 0;
    cout << "Opcje dla parametru BaudRate: " << endl;
    cout << "1. CBR_110\n"
            "2. CBR_300\n"
            "3. CBR_600\n"
            "4. CBR_1200\n"
            "5. CBR_2400\n"
            "6. CBR_4800\n"
            "7. CBR_9600\n"
            "8. CBR_14400\n"
            "9. CBR_19200\n"
            "10. CBR_38400\n"
            "11. CBR_56000\n"
            "12. CBR_57600\n"
            "13. CBR_115200\n"
            "14. CBR_128000\n"
            "15. CBR_256000\n"<< endl;


    cout << "Wybierz wartosc parametru BaudRate: ";
    cin >> baudRateOption;
    
    switch (baudRateOption) {
        case 1:
            dcb.BaudRate = CBR_110;
            break;
        case 2:
            dcb.BaudRate = CBR_300;
            break;
        case 3:
            dcb.BaudRate = CBR_600;
            break;
        case 4:
            dcb.BaudRate = CBR_1200;
            break;
        case 5:
            dcb.BaudRate = CBR_2400;
            break;
        case 6:
            dcb.BaudRate = CBR_4800;
            break;
        case 7:
            dcb.BaudRate = CBR_9600;
            break;
        case 8:
            dcb.BaudRate = CBR_14400;
            break;
        case 9:
            dcb.BaudRate = CBR_19200;
            break;
        case 10:
            dcb.BaudRate = CBR_38400;
            break;
        case 11:
            dcb.BaudRate = CBR_56000;
            break;
        case 12:
            dcb.BaudRate = CBR_57600;
            break;
        case 13:
            dcb.BaudRate = CBR_115200;
            break;
        case 14:
            dcb.BaudRate = CBR_128000;
            break;
        case 15:
            dcb.BaudRate = CBR_256000;
            break;
        default: cout << "Niepoprawna wartosc! Ustawiono domysla wartosc BoudRate na CBR_115200" << endl;
            dcb.BaudRate=CBR_115200;
            break;
    }

    int parityOption = 0;
    cout << "Opcje dla parametru Parity:\n"
            "1. NOPARITY\n"
            "2. ODDPARITY\n"
            "3. EVENPARITY\n"
            "4. MARKPARITY\n"
            "5. SPACEPARITY\n"<< endl;


    cout << "Wybierz wartosc parametru Parity: ";
    cin >> parityOption;

    switch(parityOption) {
        case 1:
            dcb.Parity = NOPARITY;
            break;
        case 2:
            dcb.Parity = ODDPARITY;
            break;
        case 3:
            dcb.Parity = EVENPARITY;
            break;
        case 4:
            dcb.Parity = MARKPARITY;
            break;
        case 5:
            dcb.Parity = SPACEPARITY;
            break;
        default: cout << "Niepoprawna wartosc! Ustawiono domysla wartosc Parity na EVENPARITY" << endl;
            dcb.Parity = 2;
            break;
    }

    int stopBitsOption = 0;
    cout << "Opcje dla parametru StopBits:\n"
            "1. ONESTOPBIT\n"
            "2. ONE5STOPBITS\n"
            "3. TWOSTOPBITS\n"<< endl;


    cout << "Wybierz wartosc parametru StopBits: ";
    cin >> stopBitsOption;

    switch(stopBitsOption) {
        case 1:
            dcb.StopBits = ONESTOPBIT;
            break;
        case 2:
            dcb.StopBits = ONE5STOPBITS;
            break;
        case 3:
            dcb.StopBits = TWOSTOPBITS;
            break;
        default: cout << "Niepoprawna wartosc! Ustawiono domysla wartosc StopBits na ONESTOPBIT" << endl;
            dcb.StopBits = ONESTOPBIT;
            break;
    }

    int byteSizeOption = 0;
    cout << "Opcje dla parametru ByteSize:\n"
            "1. 5\n"
            "2. 6\n"
            "3. 7\n"
            "4. 8\n"<< endl;

    cout << "Wybierz wartosc parametru ByteSize: ";
    cin >> byteSizeOption;

    switch(byteSizeOption) {
        case 1:
            dcb.ByteSize = 5;
            break;
        case 2:
            dcb.ByteSize = 6;
            break;
        case 3:
            dcb.ByteSize = 7;
            break;
        case 4:
            dcb.ByteSize = 8;
            break;
        default: cout << "Niepoprawna wartosc! Ustawiono domysla wartosc ByteSize na 8" << endl;
            dcb.ByteSize = 8;
            break;
    }

    SetCommState(handle, &dcb);
    printSerialPortConfig();

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

//        Konfiguracja portu
        serialPortConfig();

        SetCommState(handle, &dcb);

        GetCommTimeouts(handle, &commTimeouts);
//        Ustawienie timeoutów
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

    std::cout << "Nacisnij Enter, aby zamknac program..." << std::endl;
    std::cin.get();

    return 0;
}