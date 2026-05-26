#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <set>
#include <vector>

using namespace std;

struct Flags {
  bool show_lines = false;
  bool show_words = false;
  bool show_bytes = false;
  bool show_chars = false;
};

struct Options {
  Flags flag;
  vector<string> files;
};

struct FileStats {
  string filename;
  long lines = 0;
  long words = 0;
  long bytes = 0;
  long chars = 0;
  bool is_valid = false;
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
          cerr << "Warning: Unknown option '-" << arg[j]
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
      cerr << "Unexpected parametr `" << arg << "`. It will be ignored"
                << std::endl;
    }
  }
  opts.flag.show_lines = params.count('l');
  opts.flag.show_words = params.count('w');
  opts.flag.show_bytes = params.count('c');
  opts.flag.show_chars = params.count('m');

  // Если нет ни одной опции - выводить нужно все (кроме chars, как в wc)
  if (!opts.flag.show_lines && !opts.flag.show_words && !opts.flag.show_bytes &&
      !opts.flag.show_chars) {
    opts.flag.show_lines = true;
    opts.flag.show_words = true;
    opts.flag.show_bytes = true;
  }

  return opts;
}

FileStats count_file_stats(const string& filename) {
  FileStats stats;
  stats.filename = filename;

  ifstream file(filename, ios::binary);
  if (!file.is_open()) {
    cerr << "Error: Cannot open file '" << filename << "'" << endl;
    return stats; // return false by default
  }

  bool in_word = false;
  char ch;
  // Один проход по файлу, читаем посимвольно
  while (file.get(ch)) {
    stats.bytes++;  // Считаем байты
    stats.chars++;  // Считаем символы (все, включая спецсимволы)

    if (ch == '\n') {
      stats.lines++;
    }

    // Подсчет слов: пробельные символы разделяют слова
    if (isspace(static_cast<unsigned char>(ch))) {
      if (in_word) {
        stats.words++;
        in_word = false;
      }
    } else {
      in_word = true;
    }
  }

  // Если файл закончился на слове
  if (in_word) {
    stats.words++;
  }

  stats.is_valid = true;
  return stats;
}

void print_stats(const FileStats& stats, const Flags flags) {
  if (!stats.is_valid) return;

  if (flags.show_lines) cout << stats.lines << " ";
  if (flags.show_words) cout << stats.words << " ";
  if (flags.show_bytes) cout << stats.bytes << " ";
  if (flags.show_chars) cout << stats.chars << " ";
  
  cout << " " << stats.filename << endl;
}

int main(int argc, char** argv) {
  Options opts = parse_arguments(argc, argv);

  if (opts.files.empty()) {
    cerr << "Found no filenames to prase. \nUsage: " << argv[0]
         << " [OPTIONS] file1 [file2 ...]" << endl;
    return 1;
  }

  FileStats total;
  total.filename = "total";
  bool has_total = false;

  for (const string& filename : opts.files) {
    FileStats stats = count_file_stats(filename);

    if (stats.is_valid) {
      print_stats(stats, opts.flag);

      // Суммируем для общего итога
      if (opts.files.size() > 1) {
        total.lines += stats.lines;
        total.words += stats.words;
        total.bytes += stats.bytes;
        total.chars += stats.chars;
        has_total = true;
      }
    }
  }

  // Выводим общий итог, если было несколько файлов
  if (has_total) {
    print_stats(total, opts.flag);
  }

  return 0;
}