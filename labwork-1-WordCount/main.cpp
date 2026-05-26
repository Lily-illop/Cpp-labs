#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <set>
#include <vector>

using namespace std;

struct Options {
  bool show_lines = false;
  bool show_words = false;
  bool show_bytes = false;
  bool show_chars = false;
  vector<string> files;
};


// Парсинг аргументов командной строки
Options parse_arguments(int argc, char* argv[]) {
  Options opts;

  set<char> params = set<char>();
  for (int i = 1; i < argc; i++) {
    string arg = argv[i];

    if (arg.empty()) continue;

    if (arg[0] != '-' || arg == "-") {  // Имя файла
      opts.files.push_back(arg);
      continue;
    }

    if (arg.size() > 1 && arg[1] != '-') {
      for (int j = 1; j < arg.size(); j++) {
        if (arg[j] == 'l' || arg[j] == 'w' || arg[j] == 'c' || arg[j] == 'm')
          params.insert(arg[j]);
        else
          cout << "Warning: Unknown option '-" << arg[j]
               << "'. It will be ignored" << endl;
      }
    } else if (arg == "--lines") {
      params.insert('l');
    } else if (arg == "--bytes") {
      params.insert('c');
    } else if (arg == "--words") {
      params.insert('w');
    } else if (arg == "--chars") {
      params.insert('m');
    } else {
      std::cout << "Unexpected parametr `" << arg << "`. It will be ignored"
                << std::endl;
    }
  }
  opts.show_lines = params.count('l');
  opts.show_words = params.count('w');
  opts.show_bytes = params.count('c');
  opts.show_chars = params.count('m');

  // Если нет ни одной опции - выводить нужно все (кроме chars, как в wc)
  if (!opts.show_lines && !opts.show_words && !opts.show_bytes &&
      !opts.show_chars) {
    opts.show_lines = true;
    opts.show_words = true;
    opts.show_bytes = true;
  }

  return opts;
}

int main(int argc, char** argv) {
  Options opts = parse_arguments(argc, argv);

  if (opts.files.empty()) {
    cerr << "Found no filenames to prase. \nUsage: " << argv[0]
         << " [OPTIONS] file1 [file2 ...]" << endl;
    return 1;
  }

  vector<string> result = vector<string>();

  for (auto filename : opts.files) {
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

      if (opts.show_lines) cout << c_lines << " ";
      if (opts.show_words) cout << c_words << " ";
      if (opts.show_bytes) cout << c_bytes << " ";
      if (opts.show_chars) cout << c_chars << " ";
      cout << filename << "\n";

      file.close();
    } else {
      cout << "There is no file named \'" << filename << "\'\n";
    }
  }

  return 0;
}