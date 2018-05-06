#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <windows.h>

#include "XModem.h"

using namespace std;

char nameFile[255]; // Nazwa wysylanego pliku
char unit; // Pojedynczy znak do wyslania
char package[128]; // Blok wlasciwych danych
unsigned long unitSize = sizeof(unit); // Rozmiar pojedynczego znaku
const int unitMax = 1; // Maksymalna ilosc znakow na przesyl
int code; // Dla ustalenia trybu sumy kontrolnej
bool transfer = false; // Dotyczy proby polaczenia odbiornika i nadajnika
bool correctPackage;
int packageNumber=1; // Numer bloku
char complete255;
char checksumCRC[2];
char checksum;
char tmpchecksum;

int main (){
	ifstream file;

	LPCTSTR portName = "COM4";

	cout << "\n XModem Simulator Transmitter | Gracjan Grala / Dominika Wojcik\n";
	cout << "_________________________________________________________________\n\n";

	cout << " Starting the transfer on port " << portName << "\n";

	hPort = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	if(hPort == INVALID_HANDLE_VALUE){
		cout << " Connection error. The port is not responding.\n";
		return 0;
	}

	controlSetting.DCBlength = sizeof(controlSetting); // Wielkosc tej struktury (ustawien)

    GetCommState(hPort, &controlSetting); // Pobiera biezace ustawienia sterowania

    controlSetting.BaudRate = CBR_9600; // Predkosc transmisji
    controlSetting.fBinary = TRUE; // Wlacza tryb binarny. Windows nie obsluguje transferu w trybie niebinarnym
    controlSetting.Parity = NOPARITY; // bez bitu parzystości
    controlSetting.StopBits = ONESTOPBIT; 			// ustawienie bitu stopu (jeden bit)
    controlSetting.ByteSize = 8;  					// liczba wysyłanych bitów

    controlSetting.fParity = TRUE;
    controlSetting.fDtrControl = DTR_CONTROL_DISABLE; //Kontrola linii DTR: DTR_CONTROL_DISABLE (sygnał nieaktywny)
    controlSetting.fRtsControl = RTS_CONTROL_DISABLE; //Kontrola linii RTR: DTR_CONTROL_DISABLE (sygnał nieaktywny)
    controlSetting.fOutxCtsFlow = FALSE;
    controlSetting.fOutxDsrFlow = FALSE;
    controlSetting.fDsrSensitivity = FALSE;
    controlSetting.fAbortOnError = FALSE;
    controlSetting.fOutX = FALSE;
    controlSetting.fInX = FALSE;
    controlSetting.fErrorChar = FALSE;
    controlSetting.fNull = FALSE;

    timeSetting.ReadIntervalTimeout = 10000;
    timeSetting.ReadTotalTimeoutMultiplier = 10000;
    timeSetting.ReadTotalTimeoutConstant = 10000;
    timeSetting.WriteTotalTimeoutMultiplier = 100;
    timeSetting.WriteTotalTimeoutConstant = 100;

    SetCommState(hPort, &controlSetting); // Laczenie z portem
    SetCommTimeouts(hPort, &timeSetting);
    ClearCommError(hPort, &error ,&portInfo);

    cout << " The port " << portName << " is connected.\n";
    cout << "\n Enter the name of file to send: ";
    cin >> nameFile;

	cout<<"\n Waiting for Receiver...\n";

	for(int i=0;i<6;i++){
		// Czekamy na wybranie trybu przez odbiornik
		ReadFile(hPort, &unit,unitMax,&unitSize, NULL);

		if(unit==C){
			cout<<"\n Received: " << unit << " | CRC MODE\n\n";
			code=1;
			transfer=true;
			break;
		}
		else if(unit==NAK){
        	cout<<"\n Received: " << unit << " | BASIC MODE\n\n";
        	code=2;
        	transfer=true;
			break;
        }
	}

	// Po 6 nieudanych probach komunikacji z odbiornikiem wylaczamy program
	if(transfer==false){
		cout << "\n Timeout!\n";
		return 1;
	}

	file.open(nameFile,ios::binary);
	while(!file.eof())
	{
		cout << " Preparing the package to send.";
		// Czyscimy blok danych do wyslania
		for(int i=0;i<128;i++) package[i]=static_cast<char>(0x00);

		for(int i=0;(i<128) && (!file.eof());i++){
			package[i]=file.get();
			if(file.eof()) package[i]=ETX;
		}

		correctPackage = false; // Kontrolujemy poprawnosc przez ACK od odbiornika

		while(correctPackage == false)
		{
			cout<<" Sending the package...\n";
			// WYSYLANIE NAGLOWKA
			WriteFile(hPort, &SOH,unitMax,&unitSize, NULL); // Wysylamy SOH

			unit=static_cast<char>(packageNumber); // Wysylamy numer bloku
			WriteFile(hPort, &unit,unitMax,&unitSize, NULL);

			unit=static_cast<char>(255-packageNumber); // Wysylamy dopelnienie
			WriteFile(hPort, &unit,unitMax,&unitSize, NULL);

			// Wysylanie bloku wiadomosci
			for(int i=0;i<128;i++) WriteFile(hPort, &package[i],unitMax,&unitSize, NULL);

            if (code==2){ // Suma kontrolna dla podstawowego trybu
		       char checksum=NUL;
	           for(int i=0;i<128;i++) checksum+=package[i]%256;

	           // Wysylamy sume kontrolna
               WriteFile(hPort, &checksum,unitMax,&unitSize, NULL);
               cout << " Checksum: " << checksum << "\n";
            }
            else if(code==1){ // Suma kontrolna dla trybu CRC
				tmpCRC=CRC(package,128);
				unit=CRC2BIN(tmpCRC,1);
				WriteFile(hPort,&unit,unitMax,&unitSize, NULL);
				cout << " Checksum CRC: " << unit;
				unit=CRC2BIN(tmpCRC,2);
				cout << unit << "\n";
				WriteFile(hPort,&unit,unitMax,&unitSize, NULL);
			}

            while(true){

				unit=NUL;
				ReadFile(hPort,&unit,unitMax,&unitSize, NULL);
				// Oczekujemy na reakcje odbiornika

				if(unit==ACK){
					correctPackage=true;
					cout<<" The package has been sent correctly.\n";
					break;
				}
				if(unit==NAK){
					cout<<"\n ERROR: Sending the package failed!\n";
					break;
				}
				if(unit==CAN)
				{
					cout<<"\n ERROR: The connection was broken!\n";
					return 1;
				}
			}
		}

		// Zwiekszamy numer pakietu
		if(packageNumber==255) packageNumber=1; // Ewentualne przekrecenie licznika
		else packageNumber++;

	}
	file.close();

	while(true){
		unit=EOT; // Wysylamy znak konca transmisji
		WriteFile(hPort,&unit,unitMax,&unitSize, NULL);

		// Oczekujemy na odzew odbiornika
		ReadFile(hPort,&unit,unitMax,&unitSize, NULL);
		if(unit==ACK) break;
	}

	CloseHandle(hPort);
	cout<<"\n The file upload has been successfully completed!!!\n";

	cin.get();
	cin >> unit;
    return 0;
}
