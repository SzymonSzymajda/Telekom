#include <iostream>     // cout, cin
#include <fstream>      // ifstream
#include <vector>       // vector
#include <algorithm>    // remove, reverse

using namespace std;

//wczytujemy plik z wiadomoscia
//dzielimy wiadomosc na 8bitowe lancuchy i kodujemy kazdy z nich
//zapisujemy je do pliku
//tutaj czas na wprowadzenie bledow do pliku
//odczytujemy plik z zakodowanymi lancuchami
//odkodowujemy je, poprawiamy bledy i sklejamy wszystkie w jedna wiadomosc

void load_matrix(vector< vector<int> >&, int&, int&);
bool isZero     (const vector< vector<int> >, const vector<int>, vector<int>&, const int, const int);  // spr czy wektor bledow jest zerowy
bool correct1   (const vector< vector<int> >, vector<int>&, const vector<int>, const int, const int);  // poprawa 1 bitu
bool correct2   (const vector< vector<int> >, vector<int>&, const vector<int>, const int, const int);  // poprawa 2 bitow
void printWord  (const vector<int>, const int);
void load_word  (vector<int>&, const int);
vector<int> read_file  (ifstream& input);
void        write_file (const vector<int> msg, ofstream &output);
vector<int> encode     (const vector< vector<int> > matrix, const vector<int> msg, const int rows, const int cols);
vector<int> decode     (const vector< vector<int> >, const int, const int);

int main()
{
// wczytujemy macierz z pliku
    vector< vector<int> > matrix;
    int rows, cols;
    load_matrix(matrix, rows, cols);

// wczytujemy wiadomosc z pliku
    ifstream input;
    ofstream output;
    input.open("wiadomosc.txt");
    output.open("zakodowane.txt");
    if(!input || !output)
    {
        cout << "\nNie mozna otworzyc pliku. To koniec";
        getchar();
        exit(3);
    }

    vector<int> message = read_file(input);
    input.close();

    cout << "\nWczytana wiadomosc:         ";
    printWord( message, message.size() );

// dzielimy wiadomosc na 8 bitowe fragmenty, kodujemy każdy fragement i zapisujemy do pliku
    for(unsigned int i=0; i<message.size(); i+=8)
    {
        vector<int> msg8bit(message.begin()+i, message.begin()+i+8);
        write_file( encode(matrix, msg8bit, rows, cols), output );
    }

    output.close();

// cyk dwojeczka
    cout << "\nWcisnij [Enter] aby zdekodowac...\n"; // oczekiwanie na wprowadzenie bledow
    getchar();
    decode(matrix, rows, cols);

    cout << "\n- - -S T O P- - -";
    return 0;
}

void load_matrix(vector< vector<int> >& matrix, int& rows, int& cols)
{
    ifstream fin;
    string filename;
    filename = "dane.txt";
    //cout << "\nNazwa pliku:\t";
    //getline(cin, filename);
    fin.open(filename.c_str());
    if(!fin)
    {
        cout << "\nNie mozna otworzyc pliku. To koniec";
        getchar();
        exit(1);
    }

    fin >> rows >> cols;
    for(int i=0; i<rows; i++)
    {
        vector<int> vtemp;
        for(int j=0, temp; j<cols; j++)
        {
            fin >> temp;
            vtemp.push_back(temp);
        }
        matrix.push_back(vtemp);
    }
    fin.close();
}

bool isZero(const vector< vector<int> > matrix, const vector<int> word1, vector<int>& error, const int rows, const int cols)
{
    bool isCorrect = true;
    for (int i=0; i<rows; i++)
    {
        error[i] = 0;
        for (int j=0; j<cols; j++)
            error[i] += word1[j] * matrix[i][j];
        error[i] %= 2;

        if(error[i] == 1)
            isCorrect = false;
    }
    return isCorrect;
}

bool correct1(const vector< vector<int> > matrix, vector<int>& word1, const vector<int> error, const int cols, const int rows)
{
    for(int j=0; j<cols; j++)
        for(int i=0; i<rows; i++)
        {
            if(error[i] != matrix[i][j]) // jesli bity sa rozne to nie ma sensu sprawdzanie dalszej czesci wektorow - zostawiamy biezaca kolumne i przechodzimy do nastepnej
                break;
            if(i == rows-1) // dopiero po sprawdzeniu ze cala kolumna jest rowna wektorowi bledow mozna przystapic do poprawy bitow
            {
                word1[j] += 1;
                word1[j] %= 2;
                return true;
            }
        }
    return false;
}

bool correct2(const vector< vector<int> > matrix, vector<int>& word1, const vector<int> error, const int cols, const int rows)
{
    vector<int> temp(rows);
    for(int i=0; i<cols-1; i++)
        for(int j=i+1; j<cols; j++)
            for(int k=0; k<rows; k++)
            {
                temp[k] = (matrix[k][i] + matrix[k][j]) % 2; // suma dwoch kolumn
                if(temp[k] != error[k]) // jw w correct1
                    break;
                if(k == rows-1) // jw w correct1
                {
                    word1[i] += 1;
                    word1[i] %= 2;
                    word1[j] += 1;
                    word1[j] %= 2;
                    return true;
                }
            }
    return false;
}

void printWord(const vector<int> word, const int cols)
{
    for (int i=0; i<cols; i++)
        cout << word[i] << ' ';
    cout << endl;
}

void load_word(vector<int>& word, const int pBits)
{
    string sword0;
    getline(cin, sword0);

    sword0.erase(remove(sword0.begin(), sword0.end(), ' '), sword0.end()); // usuwamy wszystkie spacje ze stringa

    for(int i=0; i<pBits; i++)
        if(sword0[i] == '0' || sword0[i] == '1')
            word[i] = sword0[i]-'0';
        else
        {
            cout << "\nNierozpoznawany znak. To koniec";
            getchar();
            exit(2);
        }
}

vector<int> read_file(ifstream& input)
{
    vector<int> msg;

    for(int buff; input >> buff; msg.push_back(buff));

    if(msg.size()%8 != 0)
    {
        reverse(msg.begin(), msg.end()); // odwrocenie wektora zeby w koncowym efekcie pushowac na poczatek wektora
        for(unsigned int i=0; i<msg.size()%8; i++) // dobijamy zera do pelnego bajtu
            msg.push_back(0);
        reverse(msg.begin(), msg.end());
    }

    if(msg.size()%8 != 0)
    {
        cout << "\nDobijanie nie powiodlo sie";
        getchar();
        exit(5);
    }

    return msg;
}

void write_file(vector<int> msg, ofstream& output)
{
    for(unsigned int i=0; i<msg.size(); i++)
        output << msg[i] << ' ';
    output << endl;
}

vector<int> encode(const vector< vector<int> > matrix, const vector<int> msg, const int rows, const int cols)
{
    int pBits = cols - rows;
    vector<int> temp = msg;

    for(int i=0; i<rows; i++)
    {
        temp.push_back(0);
        for (int j=0; j<pBits; j++)
            temp[pBits+i] += temp[j] * matrix[i][j];
        temp[pBits+i] %= 2;
    }
    return temp;
}

vector<int> decode(const vector< vector<int> > matrix, const int rows, const int cols)
{
    //int pBits = cols - rows;
    vector<int> temp;  // z bitami parzystosci - do sprawdzenia i korekty bledow
    vector<int> temp2; // bez bitow parzystosci - do wypisania do pliku

    ifstream ifs;
    ofstream ofs;
    ifs.open("zakodowane.txt");
    ofs.open("wiadomosc2.txt");
    if(!ifs || !ofs)
    {
        cout << "\nNie mozna otworzyc pliku. To koniec";
        getchar();
        exit(6);
    }

    // wczytanie z pliku do wektorow
    for(string spom = ""; getline(ifs, spom); )
    {
        spom.erase(remove(spom.begin(), spom.end(), ' '), spom.end()); // usuwamy spacje
        //string spom2 = spom;
        //spom2.erase(spom2.end()-rows, spom2.end()); // usuwamy bity parzystosci

        for(unsigned int i=0; i<spom.size(); i++)
            temp.push_back( spom[i]-'0' );
        /*for(unsigned int i=0; i<spom2.size(); i++) //to wywalic
            temp2.push_back( spom2[i]-'0' );*/
    }

    // korekcja bledow
    vector<int> buff(cols);
    vector<int> error(rows);
    for (unsigned int i = 0; i < temp.size(); i++)
    {
        buff[i%cols] = temp[i];
        if( i%cols == (unsigned int)(cols-1) ) // cols-1 bo indeksy ida od 0
        {
            if(!isZero(matrix, buff, error, rows, cols))
            {
                if(correct1(matrix, buff, error, cols, rows))
                {
                    //tutaj dac for i robic temp2.push_back(buff[j])
                    for (int j = 0; j < cols - rows; j++)
                    {
                        temp2.push_back(buff[j]);
                    }
                }
                else if(correct2(matrix, buff, error, cols, rows))
                {
                    for (int j = 0; j < cols - rows; j++)
                    {
                        temp2.push_back(buff[j]);
                    }
                }
            }
        }
    }
    // no i chyba to bedzie tak
    // zbieram sie i wracam do lodzi, robie commita i jak chcesz to dokoncz
    // bo w brzezinach jestem

    // wypisanie do pliku
    bool ignore0 = true; // ignorujemy wiodace zera
    for(unsigned int i=0; i<temp2.size(); i++)
    {
        if(ignore0 == false)
        {
            ofs << temp2[i] << ' ';
            cout << temp2[i] << ' ';
        }
        else if(temp[i] == 0 && temp[i+1] == 1)
        {
            ignore0 = false;
        }
    }

    ofs.close();
    ifs.close();
    return temp;
}
