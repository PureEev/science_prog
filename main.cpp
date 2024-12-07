#include "Subject.h"
#include "Engine.h"

int main() {
   
   Subject subject("example.txt");

   Wrapper replace_word_wrapper(&subject, &Subject::replace_word, { {"old_word", ""}, {"new_word", ""}});
   Wrapper find_phrases_wrapper(&subject, &Subject::find_phrases, {});
   Wrapper uppercase_words_wrapper(&subject, &Subject::change_first_letters_to_uppercase, {});
   Wrapper uppercase_sentences_wrapper(&subject, &Subject::change_first_letter_in_sentence_to_uppercase, {});
   Wrapper enumerate_wrapper(&subject, &Subject::add_enumerate_to_sentence, {});

   Engine engine;

   engine.register_command(replace_word_wrapper, "replace_word");
   engine.register_command(find_phrases_wrapper, "find_phrases");
   engine.register_command(uppercase_words_wrapper, "uppercase_words");
   engine.register_command(uppercase_sentences_wrapper, "uppercase_sentences");
   engine.register_command(enumerate_wrapper, "enumerate_sentences");


   engine.execute("replace_word", { {"old_word", "text"}, {"new_word", "document"} });
   engine.execute("find_phrases", { {"phrase", "document will be used"} });
   engine.execute("uppercase_words", {});
   engine.execute("uppercase_sentences", {});
   engine.execute("enumerate_sentences", {});

    

   return 0;
}
