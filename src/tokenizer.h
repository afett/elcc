#ifndef ELCC_TOKENIZER
#define ELCC_TOKENIZER

#include <string>
#include <vector>

#include <histedit.h>

namespace elcc {

struct token_line {
	std::vector<std::string> line;
	size_t cursor_word;
	size_t cursor_offset;
};

class tokenizer {
public:
	tokenizer(std::string const& = "");
	token_line operator()(const LineInfo *li);
	~tokenizer();

private:
	void reset();
	Tokenizer *tokenizer_;
};

}
#endif
