#include <unistd.h>
#include <stdio.h>

#include <tscb/dispatch>

#include <elcc/editor.h>
#include <elcc/history.h>

namespace elcc {
namespace tscb {

/* elcc libtscb integration */
class editor : public elcc::editor {
public:
	editor(std::string const&, ::tscb::posix_reactor_service &);
	~editor();
private:
	void on_ioready(::tscb::ioready_events);
	void toggle_watch(int, bool);

	::tscb::posix_reactor_service & reactor_;
	::tscb::connection conn_;
};

editor::editor(std::string const& name, ::tscb::posix_reactor_service & reactor)
:
	elcc::editor(name, std::bind(&editor::toggle_watch, this, _1, _2)),
	reactor_(reactor),
	conn_()
{ }

void editor::toggle_watch(int fd, bool on)
{
	if (!on && conn_.connected()) {
		conn_.disconnect();
		return;
	}

	conn_ = reactor_.watch(std::bind(&editor::on_ioready, this, _1),
		fd, ::tscb::ioready_input);
}

void editor::on_ioready(::tscb::ioready_events ev)
{
	if (ev & ::tscb::ioready_input) {
		handle_io();
	}
}

editor::~editor()
{
	if (conn_.connected()) {
		conn_.disconnect();
	}
}

}}

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
	el.line_cb(std::bind(&on_line, std::ref(el), _1));

	bool stop(false);
	el.add_function("exit", "exit at eof", std::bind(&eof_handler, &reactor, &stop));
	el.bind("^D", "exit");

	el.start();

	while (!stop) {
		reactor.dispatch();
	}

	return 0;
}
