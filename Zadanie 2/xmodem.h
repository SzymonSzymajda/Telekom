#include <iostream>
#include <windows.h>

using namespace std;

const char SOH = (char)0x01;
const char EOT = (char)0x04;
const char ACK = (char)0x06;
const char NAK = (char)0x15;
const char CAN = (char)0x18;
const char C   = (char)0x43;
const char ETX = (char)0x03;
const char NUL = (char)0x00;

HANDLE hPort;             // Uchwyt dla obiektu (portu)
DCB controlSetting;       // Ustawienia sterowania szeregowego urzadzenia komunikacyjnego
COMMTIMEOUTS timeSetting; // Ustawienia dla czasu oczekiwania itp.
COMSTAT portInfo;         // Struktura zawierajaca informacje o urzadzeniu komunikacyjnym
DWORD error;
USHORT tmpCRC;

char nameFile[255];                     // Nazwa wysylanego pliku
char unit;                              // Pojedynczy znak do wyslania
char package[128];                      // Blok wlasciwych danych
unsigned long unitSize = sizeof(unit);  // Rozmiar pojedynczego znaku
const int unitMax = 1;                  // Maksymalna ilosc znakow na przesyl
int code;                               // Dla ustalenia trybu sumy kontrolnej
bool transfer = false;                  // Dotyczy proby polaczenia odbiornika i nadajnika
bool correctPackage;
int packageNumber = 1;                  // Numer bloku
char complete255;
char checksumCRC[2];
char checksum;
char tmpchecksum;

void init_port(LPCTSTR portName)
{
    hPort = CreateFile( portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL );

    if( hPort == INVALID_HANDLE_VALUE )
    {
        cout << "\nConnection error. Port is not responding";
        exit(1);
    }

    controlSetting.DCBlength = sizeof( controlSetting );

    GetCommState( hPort, &controlSetting );

    controlSetting.BaudRate = CBR_9600;
    controlSetting.fBinary = TRUE;
    controlSetting.Parity = NOPARITY;
    controlSetting.StopBits = ONESTOPBIT;
    controlSetting.ByteSize = 8;

    controlSetting.fParity = TRUE;
    controlSetting.fDtrControl = DTR_CONTROL_DISABLE;
    controlSetting.fRtsControl = RTS_CONTROL_DISABLE;
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

    SetCommState( hPort, &controlSetting );
    SetCommTimeouts( hPort, &timeSetting );
    ClearCommError( hPort, &error, &portInfo );
}

int CRC(char *wsk, int count)
{
	int checksum = 0;

	while( --count >= 0 )
    {
		checksum = checksum ^ (int)*wsk++ << 8; // Przesuniecie znaku dodajac 8 zer
		for(int i=0; i<8; ++i)
        {
			if(checksum & 0x8000)
				checksum = checksum << 1 ^ 0x1021; // Lewy bit == 1 -> XOR wielomianem CRC-16^7 (1021)
            else
                checksum = checksum << 1;          // Przesuniecie o jeden (XOR przez zera nic nie zmienia)
		}
	}

	return checksum & 0xFFFF; // Zwracamy CRC 16 bit
}

int parity(int x, int y)
{
	if     ( y == 0 )  return 1;
	else if( y == 1 )  return x;

    int temp = x;
	for(int i=2; i<=y; i++)
        temp *= x;
	return temp;
}

char crc2bin(int n, int digit)
{
	int temp, temp2;
    int bin[16];

    for(int i=0; i<16; i++)
        bin[i] = 0;

	for(int i=0; i<16; i++)
    {
		temp = n % 2;
		if     ( temp == 1 )  n = 0.5 * (n-1);
		else if( temp == 0 )  n = 0.5 * n;
        bin[15-i] = temp;
	}

	// Rozdzielamy sume kontrolna na 2 znaki 8-bitowe
	temp = 0;

	if     ( digit == 1 )  temp2 = 7;
	else if( digit == 2 )  temp2 = 15;

	for(int i=0; i<8; i++)
		temp += parity(2, i) * bin[temp2-i];

	return (char)temp;
}
