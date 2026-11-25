#include <cstddef>
#include <cstdio>
#include <fstream>
#include <string>
using namespace std;
int main() {
  ifstream file = ifstream("q2.txt");

  string line;
  int ans = 0;
  while (getline(file, line)) {
    size_t end = line.find(':');
    int id = stoi(line.substr(4, end - 4));
    ans += id;
    int start = end + 2;
    string game = line.substr(start, line.length());
    size_t p = 0;
    int n = 0;
    for (size_t i = 0; i <= game.length(); i++) {
      string substr = game.substr(p, i - p);
      if (game[i] == ' ') {
        n = stoi(substr);
        p = i + 1;
      } else if (game[i] == ';' || game[i] == ',' || i == game.length()) {
        if (substr == "red" && n > 12) {
          ans -= id;
          break;
        } else if (substr == "green" && n > 13) {
          ans -= id;
          break;
        } else if (substr == "blue" && n > 14) {
          ans -= id;
          break;
        }
        p = i++ + 2;
      }
    }
  }
  printf("%d\n", ans);
}
