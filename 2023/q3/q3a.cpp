#include <cstdio>
#include <fstream>
#include <ios>
#include <string>
using namespace std;
int main() {
  ifstream file = ifstream("q3.txt");
  file.seekg(0, iostream::end);
  streamsize size = file.tellg();
  file.seekg(0, iostream::beg);

  string input = string(size, '\0');
  file.read(&input[0], size);

  int width = input.find('\n');
  int height = 1;
  int offset = width;
  while (input.find('\n', offset += width) != string::npos) {
    height++;
  }

  int vectors[8][2] = {{1, 0},  {1, 1},   {0, 1},  {-1, 1},
                       {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};
  int ans = 0;
  int p = 0;
  bool buffered = false;
  bool touched = false;
  int w = -1, h = -1;
  for (int i = 0; i < input.length(); i++) {
    w++;
    if (w % (width + 1) == 0) {
      w = 0;
      h++;
    }
    // printf("%d %d %d %d\n", w, h, i, w + width * h);
    if ('0' <= input[i] && input[i] <= '9') {
      buffered = true;
      for (int j = 0; j < 8; j++) {
        int *vector = vectors[j];
        int x = w + vector[0], y = h + vector[1];
        if (0 <= x && x < width && 0 <= y && y < height) {
          int idx = x + (width + 1) * y;
          char c = input[idx];
          bool t = !('0' <= c && c <= '9') && !(c == '.') && !(c == '\n');
          touched = touched | t;
        }
      }
      continue;
    }
    if (buffered) {
      if (touched) {
        // ans
        string substr = input.substr(p, i - p);
        ans += stoi(&substr[0]);
        touched = false;
      }
      buffered = false; // readout
    }
    p = i + 1;
  }
  if (buffered) {
    if (touched) {
      // ans
      string substr = input.substr(p, input.length() - p);
      ans += stoi(&substr[0]);
      printf("%s\n", substr.c_str());
      touched = false;
    }
    buffered = false; // readout
  }
  printf("%d\n", ans);
}
