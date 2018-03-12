#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>

using namespace std;

//wczytywanie macierzy
//kodowanie slowa
//wczytanie slowa
//sprawdznie poprawnosci

void load_matrix(vector< vector<int> >&, int&, int&);
bool isZero(vector< vector<int> >&, vector<int>&, vector<int>&, int&, int&);
bool correct1(vector< vector<int> >&, vector<int>&, vector<int>&, int&, int&);
bool correct2(vector< vector<int> >&, vector<int>&, vector<int>&, int&, int&);

int main()
{
    cout << "- - S T A R T - -";

// wczytanie matrixa
    int rows, cols;
    vector< vector<int> > matrix;
    load_matrix(matrix, rows, cols);
    cout << rows << " x " << cols << endl;

    int pBits = cols - rows;
    int word0[cols];
    for (int i=0; i<cols; i++)
        word0[i] = 0;

//wczytaj slowo 8bit
    cout << "\nWpisz slowo:            ";
    for (int i=0; i<pBits; i++)
        cin >> word0[i];

//zakoduj slowo
    for (int i=0; i<rows; i++)
    {
        for (int j=0; j<pBits; j++)
            word0[pBits+i] += word0[j] * matrix[i][j];
        word0[pBits+i] %= 2;
    }

//wyswietl zakodowane slowo
    cout << "Zakodowane slowo:       ";
    for (int i=0; i<cols; i++)
        cout << word0[i] << " ";

// wczytaj zakodowane slowo
    cout << "\n\nPodaj zakodowane slowo: ";
    vector<int> word1;
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
        return 0;
    }
    cout << "Slowo jest niepoprawne"
         << "\nPoprawiam:              ";

// popraw slowo jesli niepoprawne
    if( correct1(matrix, word1, error, cols, rows) )
    {
        for (int i=0; i<cols; i++)
            cout << word1[i] << " ";
        cout << endl;
    }
    else if( correct2(matrix, word1, error, cols, rows) )
    {
        for (int i=0; i<cols; i++)
            cout << word1[i] << " ";
        cout << endl;
    }
    else
    {
        cout << "Za duzo bledow\n";
    }

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

bool isZero(vector< vector<int> >& matrix, vector<int>& word1, vector<int>& error, int& rows, int& cols)
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

bool correct1(vector< vector<int> >& matrix, vector<int>& word1, vector<int>& error, int& cols, int& rows)
{
    for(int j=0; j<cols; j++)
        for(int i=0; i<rows; i++)
        {
            if(error[i] != matrix[i][j])
                break;
            if(i == rows-1) // dopiero po sprawdzeniu ze cala kolumna jest rowna wektorowi bledow
            {
                word1[j] += 1;
                word1[j] %= 2;
                return true;
            }
        }
    return false;
}

bool correct2(vector< vector<int> >& matrix, vector<int>& word1, vector<int>& error, int& cols, int& rows)
{
    vector<int> temp(rows);
    for(int i=0; i<cols; i++)
    {
        for(int j=i+1; j<cols; j++)
        {
            for(int k=0; k<rows; k++)
            {
                temp[k] = (matrix[k][i] + matrix[k][j]) % 2;
                if(temp[k] != error[k])
                    break;
                if(k == rows - 1)
                {
                    word1[i] += 1;
                    word1[j] += 1;
                    word1[i] %= 2;
                    word1[j] %= 2;
                    return true;
                }
            }
        }
    }
    return false;
}
