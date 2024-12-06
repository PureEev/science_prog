#include "Subject.h"
#include <sstream>
#include <cctype>
#include <algorithm>

void Subject::open_file() {
    file_stream.open(filename, std::ios::in | std::ios::out);
    if (!file_stream.is_open()) {
        throw std::ios_base::failure("Unable to open file");
    }
}

void Subject::close_file() {
    if (file_stream.is_open()) {
        file_stream.close();
    }
}

void Subject::replace_word( std::string& old_word,  std::string& new_word) {
    open_file();

    std::string content, line;
    while (std::getline(file_stream, line)) {
        size_t pos = 0;
        while ((pos = line.find(old_word, pos)) != std::string::npos) {
            line.replace(pos, old_word.length(), new_word);
            pos += new_word.length();
        }
        content += line + '\n';
    }

    file_stream.close();
    file_stream.open(filename, std::ios::out | std::ios::trunc);
    file_stream << content;

    close_file();
}

std::string Subject::find_phrases() {
    open_file();

    std::string line;
    std::cout << "Phrases containing two or more words:\n";
    while (std::getline(file_stream, line)) {
        std::istringstream iss(line);
        std::string word, phrase;
        int word_count = 0;

        while (iss >> word) {
            if (!phrase.empty()) phrase += ' ';
            phrase += word;
            word_count++;

            if (iss.peek() == '.' || iss.peek() == ',' || iss.peek() == '\n') {
                if (word_count >= 2) {
                    std::cout << phrase << '\n';
                }
                phrase.clear();
                word_count = 0;
            }
        }
    }

    return " ";

    close_file();
}

void Subject::change_first_letters_to_uppercase() {
    open_file();

    std::string content, line;
    while (std::getline(file_stream, line)) {
        for (size_t i = 0; i < line.size(); ++i) {
            if (std::isalpha(line[i]) && (i == 0 || !std::isalpha(line[i - 1]))) {
                line[i] = std::toupper(line[i]);
            }
        }
        content += line + '\n';
    }

    file_stream.close();
    file_stream.open(filename, std::ios::out | std::ios::trunc);
    file_stream << content;
    close_file();
}

void Subject::change_first_letter_in_sentence_to_uppercase() {
    open_file();

    std::string content, line;
    bool capitalize = true;
    while (std::getline(file_stream, line)) {
        for (char& c : line) {
            if (capitalize && std::isalpha(c)) {
                c = std::toupper(c);
                capitalize = false;
            }
            if (c == '.' || c == '?' || c == '!') {
                capitalize = true;
            }
        }
        content += line + '\n';
    }

    file_stream.close();
    file_stream.open(filename, std::ios::out | std::ios::trunc);
    file_stream << content;
    close_file();
}

void Subject::add_enumerate_to_sentence() {
    open_file();

    std::string content, line;
    int sentence_number = 1;
    bool new_sentence = true;

    while (std::getline(file_stream, line)) {
        for (size_t i = 0; i < line.size(); ++i) {
            if (new_sentence) {
                content += std::to_string(sentence_number++) + ". ";
                new_sentence = false;
            }
            content += line[i];
            if (line[i] == '.' || line[i] == '?' || line[i] == '!') {
                content += '\n';
                new_sentence = true;
            }
        }
        content += '\n';
    }

    file_stream.close();
    file_stream.open(filename, std::ios::out | std::ios::trunc);
    file_stream << content;
    close_file();
}
