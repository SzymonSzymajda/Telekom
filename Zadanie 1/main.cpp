#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>

using namespace std;

void load_matrix(vector< vector<int> >&, int&, int&);

/*
H[][]       - matrix    kontrolna
T[]         - word1      wiadomosc nadana
E[]         - error     bledy
R[]=T+E                 wiadomosc odebrana
HR = HE                 spr wiadomosci odebranej
*/

int main()
{
    cout << "- - S T A R T - -";

// wczytanie matrixa
    int rows, cols;
    vector< vector<int> > matrix;
    load_matrix(matrix, rows, cols);
    cout << '\n' << rows << " x " << cols << endl;

int word0[cols];
for (int i = 0; i < cols; i++) {
    word0[i] = 0;
}
//wczytaj slowo
cout << "\nWpisz slowo:\n";
for (int i=0; i<cols-4; i++)
    cin >> word0[i];
cout << endl;

//zakoduj slowo
int pBits = cols-rows;
for (int i = 0; i < rows; i++) {
    for (int j = 0; j < pBits; j++) {
        word0[pBits+i] += word0[j] * matrix[i][j];
    }
    word0[pBits+i] %= 2;
}
//wyswietl zakodowane slowo
cout << "Zakodowane slowo: ";
for (int i = 0; i < cols; i++) {
    cout << word0[i] << " ";
}
cout << endl;

// wczytaj zakodowane slowo
    cout << "Podaj zakodowane slowo: ";
    int word1[cols];
    for (int i=0; i<cols; i++)
        cin >> word1[i];
    cout << endl;

// wektor bledow
    int error[rows];
    bool isCorrect = true;
    for (int i=0; i<rows; i++)
    {
        error[i] = 0;
        for (int j=0; j<cols; j++)
            error[i] += word1[j] * matrix[i][j];
        error[i] %= 2;
        if(error[i] == 1)
        {
            isCorrect = false;
        }
    }

    if(isCorrect)
    {
        cout << "\nSlowo jest poprawne";
        return 0;
    }
    cout << "\nSlowo jest niepoprawne";

// popraw slowo jesli niepoprawne
    bool corrected = false;
    for(int j=0; j<cols; j++)
    {
        for(int i=0; i<rows; i++)
        {
            if(error[i] != matrix[i][j])
                break;
            if(i == rows-1)
            {
                word1[j] += 1;
                word1[j] %= 2;
                corrected = true;
            }
            if(corrected)
                break;
        }
    }

// wyswietl poprawione slowo
    cout << "\n";
    for (int i=0; i<cols; i++)
    {
        cout<<word1[i]<<" ";
    }

    cout << "\n- - -S T O P- - -";
    return 0;
}

void load_matrix(vector< vector<int> >& matrix, int& rows, int& cols)
{
    ifstream fin;
    fin.open("dane.txt");
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
