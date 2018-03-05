#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char const *argv[]) {

  ifstream file;
  file.open("dane.txt");
  int rows, cols;
  file >> rows >> cols;
  int matrix[rows][cols];
  for(int i=0; i<rows; i++)
  {
    for (int  j = 0; j < cols; j++) {
      file >> matrix[i][j];
    }
  }
  file.close();
  //wczytaj slowo kodowe
  int word[cols];
  for (int i = 0; i < cols; i++) {
    cin >> word[i];
  }
  //sprawdz poprawnosc
  int error[rows];
  bool isCorrect = true;
  for (int i = 0; i < rows; i++) {
    error[i] = 0;
    for (int j = 0; j < cols; j++) {
      error[i] += word[j] * matrix[i][j];
    }
    error[i] = error[i]%2;
    if(error[i]==1) isCorrect = false;
  }

  if(isCorrect)
  {
    cout << "Slowo jest poprawne";
    return 0;
  }
  else
  {
    cout << "Slowo nie jest poprawne";
  }
  //popraw slowo jesli niepoprawne
  bool corrected = false;
  for (int j = 0; j < cols; j++) {
    for (int i = 0; i < rows; i++) {
      if(error[i]!=matrix[i][j]) break;
      if(i==rows-1)
      {
        word[j] = word[j] + 1;
        word[j] = word[j]%2;
        bool corrected = true;
      }
      if(corrected) break;
    }
  }
  //wyswietl poprawione slowo
  cout << "\n";
  for (size_t i = 0; i < cols; i++) {
    cout<<word[i]<<" ";
  }
  return 0;
}
