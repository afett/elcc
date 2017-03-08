#ifndef ELCC_TOKENIZER
#define ELCC_TOKENIZER

#include <string>

#include <histedit.h>

#include <elcc/editor.h>

namespace elcc {

class tokenizer {
public:
	tokenizer(std::string const& = "");
	token_line operator()(const LineInfo *li);
	token_line operator()(std::string const&);
	~tokenizer();

private:
	tokenizer(tokenizer const&);
	tokenizer & operator=(tokenizer const&);

	token_line handle_result(int, int, const char **, int, int);
	void reset();
	Tokenizer *tokenizer_;
};

}
#endif
