#include <cstdio>
#include <fstream>
#include <string>

using namespace std;

int main() {
  string line;
  ifstream file = ifstream("q1.txt");

  int sum = 0;
  while (getline(file, line)) {
    int f = 0, g = line.length();
    char front = line[f], back = line[g];
    bool fnum = false;
    bool bnum = false;
    while (!(fnum && bnum)) {
      fnum = '0' <= front && front <= '9';
      if (!fnum) {
        front = line[f++];
      }
      bnum = '0' <= back && back <= '9';
      if (!bnum) {
        back = line[g--];
      }
    }
    sum += (front - '0') * 10 + (back - '0'); // the config value of the line
  }
  printf("%d\n", sum);
}
