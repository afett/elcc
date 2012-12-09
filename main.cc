#include <tscb/dispatch>

#include <elcc/editor.h>

bool on_line(std::string const& line)
{
	// fprintf(stderr, "%s", line.c_str());
	return true;
}

std::string fancy_prompt()
{
	return "ok> ";
}

int main(int argc, char *argv[])
{
	setlocale(LC_CTYPE, "");
	tscb::posix_reactor reactor;
	elcc::editor el(argv[0], reactor);

	el.prompt_cb(&fancy_prompt);

	for(;;) {
		reactor.dispatch();
	}
}
