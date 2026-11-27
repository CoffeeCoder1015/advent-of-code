#include <cstdio>
#include <float.h>
#include <fstream>
#include <ios>
#include <string>
#include <unordered_map>
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
  int star_idx[8];
  int star_count = 0;

  int w = -1, h = -1;
  unordered_map<int, int[2]> stars_found;
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
          if (c == '*') {
            bool seen = false;
            for (int k = 0; k < star_count; k++) {
              if (star_idx[k] == idx) {
                seen = true;
              }
            }
            if (!seen) {
              star_idx[star_count++] = idx;
            }
            touched = true;
          }
        }
      }
      continue;
    }
    if (buffered) {
      if (touched) {
        // ans
        string substr = input.substr(p, i - p);
        int num = stoi(&substr[0]);
        for (int k = 0; k < star_count; k++) {
          int idx = star_idx[k];
          if (stars_found.find(idx) == stars_found.end()) {
            stars_found[idx][0] = 1;
            stars_found[idx][1] = num;
          } else {
            stars_found[idx][0]++;
            stars_found[idx][1] *= num;
          }
        }
        touched = false;
        star_count = 0;
      }
      buffered = false; // readout
    }
    p = i + 1;
  }
  if (buffered) {
    if (touched) {
      // ans
      string substr = input.substr(p, input.length() - p);
      int num = stoi(&substr[0]);
      for (int k = 0; k < star_count; k++) {
        int idx = star_idx[k];
        if (stars_found.find(idx) == stars_found.end()) {
          stars_found[idx][0] = 1;
          stars_found[idx][1] = num;
        } else {
          stars_found[idx][0]++;
          stars_found[idx][1] *= num;
        }
      }
      touched = false;
      star_count = 0;
    }
    buffered = false; // readout
  }
  for (const auto &pair : stars_found) {
    if (pair.second[0] == 2) {
      ans += pair.second[1];
    }
  }
  printf("%d\n", ans);
}
