#include <fstream>
#include <iostream>
#include <regex>
#include <set>
#include <vector>

using namespace std;

int main(int argc, char** argv) {
  vector<string> files;
  vector<string> result;
  set<char> params;

  for (int i = 1; i < argc; i++) {
    string now_param = string(argv[i]);

    if (now_param[0] == '-' && now_param[1] != '-') {
      for (int j = 1; j < now_param.size(); ++j) {
        if (now_param[j] != 'a' && now_param[j] != 'l' && now_param[j] != 'c' &&
            now_param[j] != 'w' && now_param[j] != 'm') {
          cout << "ATTETION: there is no param named \'" << now_param[j]
               << "\'!!!!\n Prog closed\n";
          return 0;
        }
        params.insert(now_param[j]);
      }
    } else if (now_param == "--lines") {
      params.insert('l');
    } else if (now_param == "--bytes") {
      params.insert('c');
    } else if (now_param == "--words") {
      params.insert('w');
    } else if (now_param == "--chars") {
      params.insert('m');
    } else {
      files.push_back(now_param);
    }
  }

  for (const auto& filename : files) {
    ifstream file(filename, ios::in);
    size_t c_lines = 0;

    size_t c_words = 0;
    size_t c_chars = 0;

    if (file.is_open()) {
      ifstream in(filename, ifstream::binary);
      size_t c_bytes = in.seekg(0, ios::end).tellg();
      in.close();

      char c;
      string text = "";

      while (file.get(c)) {
        if (c == '\n') c_lines++;
        text += c;
      }
      if (c != '\n') c_lines++;

      regex word_regex("(\\S+)");
      auto words_begin = sregex_iterator(text.begin(), text.end(), word_regex);
      auto words_end = sregex_iterator();
      c_words = distance(words_begin, words_end);

      regex char_regex("(\\S){1}");
      auto chars_begin = sregex_iterator(text.begin(), text.end(), char_regex);
      auto chars_end = sregex_iterator();
      c_chars = distance(chars_begin, chars_end);

      if (!params.size()) {
        cout << c_lines << " " << c_words << " " << c_bytes << " " << filename
             << "\n";
      } else {
        if (params.count('a')) cout << 1.0 * c_bytes / c_words << " ";
        if (params.count('l')) cout << c_lines << " ";
        if (params.count('w')) cout << c_words << " ";
        if (params.count('c')) cout << c_bytes << " ";
        if (params.count('m')) cout << c_words << " ";
        cout << filename << "\n";
      }

    } else {
      cout << "There is no file named \'" << filename << "\'\n";
    }
  }

  return 0;
}