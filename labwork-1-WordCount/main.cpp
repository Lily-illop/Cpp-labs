#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <set>
#include <vector>

using namespace std;

int main(int argc, char** argv) {
  vector<string> files = vector<string>();
  vector<string> result = vector<string>();
  set<char> params = set<char>();
  for (int i = 1; i < argc; i++) {
    string now_param = string(argv[i]);

    if (now_param[0] == '-' && now_param[1] != '-') {
      for (int j = 1; j < now_param.size(); j++) {
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

  for (auto filename : files) {
    ifstream file(filename, ios::in);
    long int c_lines = 0;

    long int c_words = 0;
    long int c_chars = 0;

    if (file.is_open()) {
      std::ifstream in(filename, ifstream::binary);
      long int c_bytes = in.seekg(0, ios::end).tellg();
      in.close();

      string line;
      string text = "";
      while (getline(file, line)) {
        c_lines++;
        text += line + "\n";
      }
      regex word_regex("(\\w+)");
      auto words_begin = sregex_iterator(text.begin(), text.end(), word_regex);
      auto words_end = sregex_iterator();
      c_words = distance(words_begin, words_end);

      regex word_regex1("[A-Za-z]{1}");
      words_begin = sregex_iterator(text.begin(), text.end(), word_regex1);
      words_end = sregex_iterator();
      c_chars = distance(words_begin, words_end);

      if (!params.size()) {
        cout << c_lines << " " << c_words << " " << c_bytes << " " << filename
             << "\n";
      } else {
        if (params.count('l')) cout << c_lines << " ";
        if (params.count('w')) cout << c_words << " ";
        if (params.count('c')) cout << c_bytes << " ";
        if (params.count('m')) cout << c_words << " ";
        cout << filename << "\n";
      }

      file.close();
    } else {
      cout << "There is no file named \'" << filename << "\'\n";
    }
  }

  return 0;
}