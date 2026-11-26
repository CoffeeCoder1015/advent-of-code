#include <cstddef>
#include <cstdio>
#include <fstream>
#include <string>
using namespace std;
int main() {
  ifstream file = ifstream("q2.txt");

  size_t ans = 0;
  string line;
  while (getline(file, line)) {
    size_t end = line.find(':');
    int start = end + 2;
    string game = line.substr(start, line.length());

    int r = 0, g = 0, b = 0;
    size_t p = 0;
    int n = 0;
    for (size_t i = 0; i <= game.length(); i++) {
      string substr = game.substr(p, i - p);
      if (game[i] == ' ') {
        n = stoi(substr);
        p = i + 1;
      } else if (game[i] == ';' || game[i] == ',' || i == game.length()) {
        if (substr == "red") {
          r = max(r, n);
        } else if (substr == "green") {
          g = max(g, n);
        } else {
          b = max(b, n);
        }
        p = i++ + 2;
      }
    }

    ans += r * g * b;
  }
  printf("%zu\n", ans);
}
