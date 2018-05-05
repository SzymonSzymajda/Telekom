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

int main(){
	ofstream file;

	LPCTSTR portName = "COM2";

	cout << "\n XModem Simulator Receiver | Gracjan Grala / Dominika Wojcik\n";
	cout << "______________________________________________________________\n\n";

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

	SetCommState(hPort, &controlSetting);
	SetCommTimeouts(hPort, &timeSetting);
	ClearCommError(hPort, &error ,&portInfo);

	cout << " The port " << portName << " is connected.\n";
	cout << "\n Enter the name of file for save message: ";
	cin >> nameFile;
	code = 0;

	while(code!=1 && code!=2){
		cout << "\n Select the type of checksum [1 - CRC | 2 - BASIC]: ";
		cin >> code;
	};

	for(int i=0;i<6;i++){
		// Wysylanie rodzaju sumy kontrolnej
		if(code==1) unit=C; else unit=NAK;
		WriteFile(hPort, &unit,unitMax,&unitSize, NULL);

		// Czekamy na SOH od nadajnika
		cout<<" Waiting for packages from the transmitter...\n";
		ReadFile(hPort, &unit,unitMax,&unitSize, NULL);

		if(unit==SOH){
			cout << " Connected with Transmitter!\n";
			transfer = true;
			break;
		}
	}

	// Brak SOH
	if(transfer == false){
		cout << " ERROR: Connection failed!\n";
		return 1;
	}

	file.open(nameFile,ios::binary);
	cout<<" Waiting to receive packages.";

	ReadFile(hPort, &unit,unitMax,&unitSize, NULL);
	packageNumber=static_cast<int>(unit);

	ReadFile(hPort, &unit,unitMax,&unitSize, NULL);
	complete255=unit;

	for(int i=0;i<128;i++){
		ReadFile(hPort, &unit,unitMax,&unitSize, NULL);
		package[i]=unit;
	}

	if(code==1){
		ReadFile(hPort, &unit,unitMax,&unitSize, NULL);
		checksumCRC[0]=unit;
		ReadFile(hPort, &unit,unitMax,&unitSize, NULL);
		checksumCRC[1]=unit;
		correctPackage=true;
	}

	if(code==2){
		ReadFile(hPort, &unit,unitMax,&unitSize, NULL);
		checksum=unit;
		correctPackage=true;
	}

	if( static_cast<char>(255-packageNumber)!=complete255 ){
		cout << " ERROR: The wrong package number was received.!\n";
		WriteFile(hPort, &NAK,unitMax,&unitSize, NULL);
		correctPackage=false;
	}

	if(code==1){
		tmpCRC=CRC(package,128);	// Sprawdzenie sum kontrolnych CRC
		if(CRC2BIN(tmpCRC,1)!=checksumCRC[0] || CRC2BIN(tmpCRC,2)!=checksumCRC[1]){
			cout<<" ERROR: Invalid checksum!\n";
			WriteFile(hPort, &NAK,unitMax,&unitSize, NULL);
			correctPackage=false;
		}
	}

	if(code==2){
		tmpchecksum=NUL; // Sprawdzenie podstawowej sumy kontrolnej
		for(int i=0;i<128;i++) tmpchecksum+=package[i]%256;
		if(tmpchecksum!=checksum){
			cout<<" ERROR: Invalid checksum!\n";
			WriteFile(hPort, &NAK,unitMax,&unitSize, NULL);
			correctPackage=false;
		}
	}

	if(correctPackage){
		for(int i=0;i<128;i++){
				if(package[i]!=ETX)	file << package[i];
		}
		cout<<" Receiving the package successfully!\n";
		WriteFile(hPort, &ACK,unitMax,&unitSize, NULL);
	}

	while(true){
		ReadFile(hPort, &unit,unitMax,&unitSize, NULL);
		if(unit==EOT || unit==CAN) break;

		ReadFile(hPort, &unit,unitMax,&unitSize, NULL);
		packageNumber=static_cast<int>(unit);

		ReadFile(hPort, &unit,unitMax,&unitSize, NULL);
		complete255=unit;

		for(int i=0;i<128;i++){
			ReadFile(hPort, &unit,unitMax,&unitSize, NULL);
			package[i]=unit;
		}

		if(code==1){
			ReadFile(hPort, &unit,unitMax,&unitSize, NULL);
			checksumCRC[0]=unit;
			ReadFile(hPort, &unit,unitMax,&unitSize, NULL);
			checksumCRC[1]=unit;
			correctPackage=true;
		}

		if(code==2){
			ReadFile(hPort, &unit,unitMax,&unitSize, NULL);
			checksum=unit;
			correctPackage=true;
		}

	    if( static_cast<char>(255-packageNumber)!=complete255 ){
			cout << " ERROR: The wrong package number was received.!\n";
			WriteFile(hPort, &NAK,unitMax,&unitSize, NULL);
			correctPackage=false;
		}


		if(code==1){
			tmpCRC=CRC(package,128);	// Sprawdzenie sum kontrolnych CRC
			if(CRC2BIN(tmpCRC,1)!=checksumCRC[0] || CRC2BIN(tmpCRC,2)!=checksumCRC[1]){
				cout<<" ERROR: Invalid checksum!\n";
				WriteFile(hPort, &NAK,unitMax,&unitSize, NULL);
				correctPackage=false;
			}
		}

		if(code==2){
			tmpchecksum=NUL; // Sprawdzenie podstawowej sumy kontrolnej
			for(int i=0;i<128;i++) tmpchecksum+=package[i]%256;
			if(tmpchecksum!=checksum){
				cout<<" ERROR: Invalid checksum!\n";
				WriteFile(hPort, &NAK,unitMax,&unitSize, NULL);
				correctPackage=false;
			}
		}

		if(correctPackage){
			for(int i=0;i<128;i++){
					if(package[i]!=ETX)	file << package[i];
					else break;
			}
			cout<<" Receiving the package successfully!\n";
			WriteFile(hPort, &ACK,unitMax,&unitSize, NULL);
		}
	}
	WriteFile(hPort, &ACK,unitMax,&unitSize, NULL);

	file.close();
	CloseHandle(hPort);

	if(unit==CAN) cout << "\n ERROR: The connection was broken!\n";
	else cout << "\n File downloaded successfully!\n";

	cin.get();
	cin >> unit;
	return 0;
}
