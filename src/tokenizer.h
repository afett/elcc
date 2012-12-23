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
	~tokenizer();

private:
	tokenizer(tokenizer const&);
	tokenizer & operator=(tokenizer const&);

	void reset();
	Tokenizer *tokenizer_;
};

}
#endif
