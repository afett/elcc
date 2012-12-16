#include <unistd.h>
#include <stdio.h>

#include <boost/bind.hpp>
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

void exit_editor(tscb::posix_reactor_service *reactor, bool *stop)
{
	*stop = true;
	reactor->get_eventtrigger().set();
}

int main(int argc, char *argv[])
{
	tscb::posix_reactor reactor;

	setlocale(LC_CTYPE, "");
	elcc::editor el(argv[0], reactor);

	el.prompt_cb(&fancy_prompt);
	el.line_cb(&on_line);

	bool stop(false);
	el.eof_cb(boost::bind(&exit_editor, &reactor, &stop));

	el.run();

	while (!stop) {
		reactor.dispatch();
	}
}
