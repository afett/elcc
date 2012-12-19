#include <unistd.h>
#include <stdio.h>

#include <boost/bind.hpp>
#include <tscb/dispatch>

#include <elcc/tscb/editor.h>

void on_line(std::string const&)
{
}

std::string fancy_prompt()
{
	return "ok> ";
}

elcc::function_return eof_handler(tscb::posix_reactor_service *reactor, bool *stop)
{
	*stop = true;
	reactor->get_eventtrigger().set();
	return elcc::eof;
}

int main(int argc, char *argv[])
{
	tscb::posix_reactor reactor;
	tscb::connection conn;

	setlocale(LC_CTYPE, "");
	elcc::tscb::editor el("elcc", reactor);

	el.prompt_cb(&fancy_prompt);
	el.line_cb(&on_line);

	bool stop(false);
	el.add_function("exit", "exit at eof", boost::bind(&eof_handler, &reactor, &stop));
	el.bind("^D", "exit");

	el.run();

	while (!stop) {
		reactor.dispatch();
	}
}
