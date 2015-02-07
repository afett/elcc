#include <unistd.h>
#include <stdio.h>

#include <boost/bind.hpp>
#include <tscb/dispatch>

#include <elcc/tscb/editor.h>
#include <elcc/history.h>

void on_line(elcc::tscb::editor & el, std::string const& line)
{
	el.history().enter(line);
}

std::string fancy_prompt()
{
	return "ok> ";
}

elcc::function_return eof_handler(tscb::posix_reactor_service *reactor, bool *stop)
{
	*stop = true;
	reactor->get_eventtrigger().set();
	fprintf(stdout, "\n");
	return elcc::eof;
}

int main()
{
	tscb::posix_reactor reactor;
	tscb::connection conn;

	setlocale(LC_CTYPE, "");
	elcc::tscb::editor el("elcc", reactor);

	el.prompt_cb(&fancy_prompt);
	el.line_cb(boost::bind(&on_line, boost::ref(el), _1));

	bool stop(false);
	el.add_function("exit", "exit at eof", boost::bind(&eof_handler, &reactor, &stop));
	el.bind("^D", "exit");

	el.start();

	while (!stop) {
		reactor.dispatch();
	}

	return 0;
}
