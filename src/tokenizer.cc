#include <stdexcept>

#include <tokenizer.h>
namespace elcc {

tokenizer::tokenizer(std::string const& ifs) :
	tokenizer_(tok_init(ifs.empty() ? NULL : ifs.c_str()))
{
	if (!tokenizer_) {
		throw std::bad_alloc();
	}
}

void tokenizer::reset()
{ tok_reset(tokenizer_); }

token_line tokenizer::operator()(const LineInfo *li)
{
	const char **argv;
	int argc, cursorc, cursoro;

	int ret(tok_line(tokenizer_, li, &argc, &argv, &cursorc, &cursoro));
	if (ret != 0) {
		reset();
		throw std::runtime_error("failed to parse line");
	}

	token_line res;
	for (int i(0); i < argc; ++i) {
		res.line.push_back(argv[i]);
	}

	res.cursor_word = cursorc;
	res.cursor_offset = cursoro;

	reset();
	return res;
}

tokenizer::~tokenizer()
{
	tok_end(tokenizer_);
}

}
