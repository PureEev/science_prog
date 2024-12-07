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

void Subject::find_phrases(const std::string& phrase) {
    open_file();

    std::string line;
    int lineNumber = 0;
    bool found = false;

    while (std::getline(file_stream, line)) { 
        lineNumber++;
        size_t position = line.find(phrase); 

        if (position != std::string::npos) { 
            std::cout << "Phrase found on line " << lineNumber << std::endl;
            found = true;
        }
    }

    if (!found) {
        std::cout << "Phrase not found." << std::endl;
    }

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

    std::string content, line, current_line;
    int sentence_number = 1;
    bool new_sentence = false;


    while (std::getline(file_stream, line)) {
        for (size_t i = 0; i < line.size(); ++i) {
            if (new_sentence) {
                size_t first_not_space = current_line.find_first_not_of(' ');

                if (first_not_space != std::string::npos) {
                    current_line.erase(0, first_not_space);
                }
                content += std::to_string(sentence_number++) + ". ";
                content += current_line;
                content += '\n';
                new_sentence = false;
                current_line.clear();
            }

            current_line += line[i];
            if (line[i] == '.' || line[i] == '?' || line[i] == '!') {
                new_sentence = true;
            }
        }
    }

    file_stream.close();
    file_stream.open(filename, std::ios::out | std::ios::trunc);
    file_stream << content;
    close_file();
}
