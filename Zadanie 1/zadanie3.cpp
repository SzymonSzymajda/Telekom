#include <iostream>     // cout, cin
#include <fstream>      // ifstream
#include <vector>       // vector
#include <algorithm>    // remove

using namespace std;

//wczytywanie macierzy
//kodowanie slowa
//wczytanie slowa
//sprawdznie poprawnosci

void load_matrix(vector< vector<int> >& matrix, int& rows, int& cols);
bool isZero     (const vector< vector<int> >, const vector<int>, vector<int>&, const int, const int);  // spr czy wektor bledow jest zerowy
bool correct1   (const vector< vector<int> >, vector<int>&, const vector<int>, const int, const int);  // poprawa 1 bitu
bool correct2   (const vector< vector<int> >, vector<int>&, const vector<int>, const int, const int);  // poprawa 2 bitow
void printWord  (const vector<int>, const int);
void load_word  (vector<int>&, const int);
vector<int> encode(const vector< vector<int> > matrix, const vector<int> msg, const int rows, const int cols);
vector<int> read_from_file(ifstream& input);
void write_to_file(vector<int> msg, ofstream &output);


int main() {
    //wczytujemy macierz z pliku
    //wczytujemy wiadomosc z pliku
    //dzielimy wiadomosc na 8 bitowe fragmenty
    //kodujemy każdy fragement i zapisujemy do pliku
    vector< vector<int> > matrix;
    int rows, cols;
    load_matrix(matrix, rows, cols);
    ifstream input;
    input.open("wiadomosc.txt");
    ofstream output;
    output.open("zakodowane.txt");
    vector<int> message = read_from_file(input);
    vector<int> msg(8);
    for (size_t i = 0; i < message.size(); i++) {
        for (size_t j = 0; j < 8; j++) {
            msg[j] = message[i];
        }
        write_to_file(encode(matrix, msg, rows, cols), output);
    }
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
        error.push_back(0);
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
        cout << word[i] << " ";
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

vector<int> encode(const vector< vector<int> > matrix, const vector<int> msg, const int rows, const int cols){
    int pBits = cols - rows;
    vector<int> temp = msg;
    for (int i = 0; i < rows; i++) {
        temp.push_back(0);
        for (int j = 0; j < pBits; j++) {
            temp[pBits+i] = temp[j] * matrix[i][j];
        }
        temp[pBits+1] %= 2;
    }
    return temp;
}

vector<int> read_from_file(ifstream& input){
    vector<int> msg;
    int buff;
    while(!input.eof()){
        input >> buff;
        msg.push_back(buff);
    }
    input.close();
    if(msg.size()%8!=0){
        for (int i = 0; i < 8-msg.size()%8; i++) {
            msg.push_back(0);
        }
    }
    return msg;
}

void write_to_file(vector<int> msg, ofstream& output){
    for(int bit : msg){
        output << bit;
    }
    output.close();
}
