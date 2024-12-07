#pragma once
#include <iostream>
#include <string>
#include <fstream>


class Subject {

private:

	std::string filename;

	std::fstream file_stream;

	void open_file();

	void close_file();

public:

	Subject(const std::string& file) : filename(file) {}

	void replace_word( std::string& old_word,  std::string& new_word);

	void find_phrases(const std::string& phrase);

	void change_first_letters_to_uppercase();

	void change_first_letter_in_sentence_to_uppercase();

	void add_enumerate_to_sentence();
};