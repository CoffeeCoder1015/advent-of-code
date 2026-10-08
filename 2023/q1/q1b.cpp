#include <cassert>
#include <cstdio>
#include <fstream>
#include <string>

using namespace std;

const string numbers[] = {"one", "two",   "three", "four", "five",
                          "six", "seven", "eight", "nine"};

int main() {
  string line;
  ifstream file = ifstream("q1.txt");

  int sum = 0;
  while (getline(file, line)) {
    int first = -1;
    for (int i = 0; i < line.length(); i++) {
      if ('0' <= line[i] && line[i] <= '9') {
        first = line[i] - '0';
        break;
      }
      for (int j = 0; j < 9; j++) {
        string strnum = numbers[j];
        string cmpstr = line.substr(i, strnum.length());
        if (strnum.compare(cmpstr) == 0) {
          first = j + 1;
          break;
        }
      }
      if (first != -1) {
        break;
      }
    }

    int last = -1;
    for (int i = line.length() - 1; i >= 0; i--) {
      if ('0' <= line[i] && line[i] <= '9') {
        last = line[i] - '0';
        break;
      }
      for (int j = 0; j < 9; j++) {
        string strnum = numbers[j];
        if (int(i - strnum.length() + 1) < 0) {
          continue;
        }
        string cmpstr = line.substr(i - strnum.length() + 1, strnum.length());
        if (strnum.compare(cmpstr) == 0) {
          last = j + 1;
          break;
        }
      }
      if (last != -1) {
        break;
      }
    }
    sum += first * 10 + last;
  }
  printf("%d\n", sum);
}
