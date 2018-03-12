#include <iostream>     // cout, cin
#include <fstream>      // ifstream
#include <vector>       // vector
#include <string>       // string
#include <cstdlib>      // exit
#include <cstdio>       // getchar

using namespace std;

//wczytywanie macierzy
//kodowanie slowa
//wczytanie slowa
//sprawdznie poprawnosci

void load_matrix(vector< vector<int> >&, int&, int&);
bool isZero     (const vector< vector<int> >, const vector<int>, vector<int>&, const int, const int);  // spr czy wektor bledow jest zerowy
bool correct1   (const vector< vector<int> >, vector<int>&, const vector<int>, const int, const int);  // poprawa 1 bitu
bool correct2   (const vector< vector<int> >, vector<int>&, const vector<int>, const int, const int);  // poprawa 2 bitow
void printWord  (const vector<int>, const int);

int main()
{
    cout << "- - S T A R T - -";

// wczytanie matrixa i jego rozmiarow
    int rows, cols;
    vector< vector<int> > matrix;

    load_matrix(matrix, rows, cols);
    cout << "macierz " << rows << " x " << cols << endl;

    int pBits = cols - rows;
    int word0[cols];

// wczytaj slowo surowe
    cout << "\nWpisz slowo:            ";
    for(int i=0; i<pBits; i++)
        cin >> word0[i];

// zakoduj slowo - dopisz bity parzystosci
    for(int i=0; i<rows; i++)
    {
        word0[pBits+i] = 0; // wyzerowanie niezainicjalizowanych pol tablicy
        for (int j=0; j<pBits; j++)
            word0[pBits+i] += word0[j] * matrix[i][j];
        word0[pBits+i] %= 2;
    }

// wyswietl zakodowane slowo
    cout << "Zakodowane slowo:       ";
    for (int i=0; i<cols; i++)
        cout << word0[i] << " ";

// wczytaj zakodowane slowo (z bitami parzystosci)
    vector<int> word1;

    cout << "\n\nPodaj zakodowane slowo: ";
    for (int i=0, tmp; i<cols; i++)
    {
        cin >> tmp;
        word1.push_back(tmp);
    }

// wektor bledow
    vector<int> error;

    if( isZero(matrix, word1, error, rows, cols) )
    {
        cout << "\nSlowo jest poprawne";
        getchar();
        return 0;
    }
    cout << "Slowo jest niepoprawne"
         << "\nPoprawiam:              ";

// popraw slowo jesli jest niepoprawne
    if     ( correct1(matrix, word1, error, cols, rows) )  printWord(word1, cols);
    else if( correct2(matrix, word1, error, cols, rows) )  printWord(word1, cols);
    else                                                   cout << "Za duzo bledow\n";

    cout << "\n- - -S T O P- - -";
    return 0;
}

void load_matrix(vector< vector<int> >& matrix, int& rows, int& cols)
{
    ifstream fin;
    string filename;
    filename = "dane1.txt";
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
    for(int i=0; i<cols; i++)
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

void printWord(const vector<int> word1, const int cols)
{
    for (int i=0; i<cols; i++)
        cout << word1[i] << " ";
    cout << endl;
}
