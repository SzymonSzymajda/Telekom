#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <windows.h>
#include "xmodem.h"

using namespace std;

void receiver(string);
void transmitter(string);

int main()
{
    cout << "\n- - S T A R T - -\n";
    char pick;
    string name;

    cout << "\nChoose mode:"
         << "\n[1] Receiver"
         << "\n[2] Transmitter       ";
    cin >> pick;

    cout << "\nPort name:            ";
    cin >> name;

    if     ( pick == '1' )  receiver( name );
    else if( pick == '2' )  transmitter( name );
    else                    exit(-1);

    cout << "\n- - -S T O P- - -\n";
    return 0;
}

void receiver(string name)
{
    ofstream file;

    LPCTSTR portName = name.c_str();

    cout << "\nTransfer started";

    init_port( portName );

    cout << "\nPort is connected"
         << "\nEnter filename to save received message:   ";
    cin >> nameFile;
    code = 0;

    while( code != 1 && code != 2 )
    {
        cout << "\nType of checksum"
             << "\n[1] CRC"
             << "\n[2] basic       ";
        cin >> code;
    };

    for(int i=0; i<6; i++)
    {
        if( code == 1 )
            unit = C;
        else
            unit = NAK;
        WriteFile( hPort, &unit, unitMax, &unitSize, NULL );

        cout << "\nWaiting for transmission";
        ReadFile( hPort, &unit, unitMax, &unitSize, NULL );

        if( unit == SOH )
        {
            cout << "\nConnection successful";
            transfer = TRUE;
            break;
        }
    }

    if( transfer == FALSE )
    {
        cout << "\nTimeout";
        exit(2);
    }

    file.open( nameFile, ios::binary );
    cout << "\nWaiting to receive packages";

    ReadFile( hPort, &unit, unitMax, &unitSize, NULL );
    packageNumber = (int)unit;

    ReadFile( hPort, &unit, unitMax, &unitSize, NULL );
    complete255 = unit;

    for(int i=0; i<128; i++)
    {
        ReadFile( hPort, &unit, unitMax, &unitSize, NULL );
        package[i] = unit;
    }

    if( code == 1 )
    {
        ReadFile( hPort, &unit, unitMax, &unitSize, NULL );
        checksumCRC[0] = unit;
        ReadFile( hPort, &unit, unitMax, &unitSize, NULL );
        checksumCRC[1] = unit;
        correctPackage = TRUE;
    }
    else if( code == 2 )
    {
        ReadFile( hPort, &unit, unitMax, &unitSize, NULL );
        checksum = unit;
        correctPackage = TRUE;
    }

    if( (char)(255-packageNumber) != complete255 )
    {
        cout << "\nWrong package number was received";
        WriteFile( hPort, &NAK, unitMax, &unitSize, NULL );
        correctPackage = FALSE;
    }

    if( code == 1 )
    {
        tmpCRC = CRC( package, 128 );
        if( crc2bin( tmpCRC, 1 ) != checksumCRC[0] || crc2bin( tmpCRC, 2 ) != checksumCRC[1] )
        {
            cout << "\nInvalid checksum";
            WriteFile( hPort, &NAK, unitMax, &unitSize, NULL );
            correctPackage = FALSE;
        }
    }
    else if( code == 2 )
    {
        tmpchecksum = NUL;
        for(int i=0; i<128; i++)
            tmpchecksum += package[i] % 256;
        if( tmpchecksum != checksum)
        {
            cout << "\nInvalid checksum";
            WriteFile( hPort, &NAK, unitMax, &unitSize, NULL );
            correctPackage = FALSE;
        }
    }

    if( correctPackage )
    {
        for(int i=0; i<128; i++)
            if( package[i] != ETX )
                file << package[i];

        cout << "\nReceiving successful";
        WriteFile( hPort, &ACK, unitMax, &unitSize, NULL );
    }

    while(TRUE)
    {
        ReadFile( hPort, &unit, unitMax, &unitSize, NULL );
        if( unit == EOT || unit == CAN )
            break;

        ReadFile( hPort, &unit, unitMax, &unitSize, NULL );
        packageNumber = (int)unit;

        ReadFile( hPort, &unit, unitMax, &unitSize, NULL );
        complete255 = unit;

        for(int i=0; i<128; i++)
        {
            ReadFile( hPort, &unit, unitMax, &unitSize, NULL );
            package[i] = unit;
        }

        if( code == 1 )
        {
            ReadFile( hPort, &unit, unitMax, &unitSize, NULL );
            checksumCRC[0] = unit;
            ReadFile( hPort, &unit, unitMax, &unitSize, NULL );
            checksumCRC[1] = unit;
            correctPackage = TRUE;
        }
        else if( code == 2 )
        {
            ReadFile( hPort, &unit, unitMax, &unitSize, NULL );
            checksum = unit;
            correctPackage = TRUE;
        }

        if( (char)(255-packageNumber) != complete255 )
        {
            cout << "\nWrong package number was received";
            WriteFile( hPort, &NAK, unitMax, &unitSize, NULL );
            correctPackage = FALSE;
        }

        if( code == 1 )
        {
            tmpCRC = CRC( package, 128 );
            if( crc2bin( tmpCRC, 1 ) != checksumCRC[0] || crc2bin( tmpCRC, 2 ) != checksumCRC[1] )
            {
                cout << "\nInvalid checksum";
                WriteFile( hPort, &NAK, unitMax, &unitSize, NULL );
                correctPackage = FALSE;
            }
        }
        else if( code == 2 )
        {
            tmpchecksum = NUL;
            for(int i=0; i<128; i++)
                tmpchecksum += package[i] % 256;

            if( tmpchecksum != checksum )
            {
                cout << "\nInvalid checksum";
                WriteFile( hPort, &NAK, unitMax, &unitSize, NULL );
                correctPackage = FALSE;
            }
        }

        if( correctPackage )
        {
            for(int i=0; i<128; i++)
            {
                if( package[i] != ETX )
                    file << package[i];
                else
                    break;
            }

            cout << "\nReceiving successful";
            WriteFile( hPort, &ACK, unitMax, &unitSize, NULL );
        }
    }

    WriteFile( hPort, &ACK, unitMax, &unitSize, NULL );

    file.close();
    CloseHandle( hPort );

    if( unit == CAN )
        cout << "\nConnection broken";
    else
        cout << "\nFile downloaded successfully";
}

void transmitter(string name)
{
    ifstream file;

    LPCTSTR portName = name.c_str();

    cout << "\nTransfer started";

    init_port( portName );

    cout << "\nPort is connected"
         << "\nEnter filename to send:   ";
    cin >> nameFile;

    cout << "\nWaiting for receiver";

    for(int i=0; i<6; i++)
    {
        ReadFile( hPort, &unit, unitMax, &unitSize, NULL );

        if( unit == C )
        {
            cout << "\nReceived " << unit << " | CRC mode";
            code = 1;
            transfer = TRUE;
            break;
        }
        else if( unit == NAK )
        {
            cout << "\nReceived " << unit << " | basic mode";
            code = 2;
            transfer = TRUE;
            break;
        }
    }

    if( transfer == FALSE )
    {
        cout << "\nTimeout";
        exit(4);
    }

    file.open( nameFile, ios::binary );

    while( !file.eof() )
    {
        cout << "\nPrepering package to send";

        for(int i=0; i<128; i++)
            package[i] = (char)0x00;

        for(int i=0; i<128 and !file.eof(); i++)
        {
            package[i] = file.get();
            if( file.eof() )
                package[i] = ETX;
        }

        correctPackage = FALSE;

        while( correctPackage == FALSE )
        {
            cout << "\nSending the package";

            WriteFile( hPort, &SOH, unitMax, &unitSize, NULL );

            unit = (char)packageNumber;
            WriteFile( hPort, &unit, unitMax, &unitSize, NULL );

            unit = (char)(255-packageNumber);
            WriteFile( hPort, &unit, unitMax, &unitSize, NULL );

            for(int i=0; i<128; i++)
                WriteFile( hPort, &package[i], unitMax, &unitSize, NULL );

            if( code == 1 )
            {
                tmpCRC = CRC( package, 128 );
                unit = crc2bin( tmpCRC, 1 );
                WriteFile( hPort, &unit, unitMax, &unitSize, NULL );
                cout << "\nChecksum CRC: " << unit;
                unit = crc2bin( tmpCRC, 2 );
                cout << unit;
                WriteFile( hPort, &unit, unitMax, &unitSize, NULL );
            }
            else if( code == 2 )
            {
                char checksum = NUL;
                for(int i=0; i<128; i++)
                    checksum += package[i] % 256;

                WriteFile( hPort, &checksum, unitMax, &unitSize, NULL );
                cout << "\nChecksum " << checksum;
            }

            while(TRUE)
            {
                unit = NULL;
                ReadFile( hPort, &unit, unitMax, &unitSize, NULL );

                if( unit == ACK )
                {
                    correctPackage = true;
                    cout << "\nPackage sent successfully";
                    break;
                }
                else if( unit == NAK )
                {
                    cout << "\nSending failed";
                    break;
                }
                else if( unit == CAN )
                {
                    cout << "\nCommunication broken";
                    exit(5);
                }
            }
        }

        if( packageNumber == 255 )
            packageNumber = 1;
        else
            packageNumber++;
    }

    file.close();

    while(TRUE)
    {
        unit = EOT;
        WriteFile( hPort, &unit, unitMax, &unitSize, NULL );

        ReadFile( hPort, &unit, unitMax, &unitSize, NULL );
        if( unit == ACK )
            break;
    }

    CloseHandle( hPort );
    cout << "\nFile upload successful";
}
