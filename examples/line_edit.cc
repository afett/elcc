#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <unistd.h>

#include <iostream>
#include <map>
#include <vector>

#include <boost/bind.hpp>

#include <elcc/completion.h>
#include <elcc/editor.h>
#include <elcc/history.h>


// dumb poll based event loop, for example purposes _only_
namespace dumb_ev {

struct watch {
	typedef boost::function<void(short)> callback;

	watch()
		:
			fd(-1),
			events(0),
			cb(0)
	{ }

	watch(int fd_, short events_, callback const& cb_)
		:
			fd(fd_),
			events(events_),
			cb(cb_)
	{ }

	struct pollfd pollfd() const
	{
		struct pollfd pfd;
		pfd.fd = fd;
		pfd.events = events;
		return pfd;
	}

	int fd;
	short events;
	callback cb;
};

class loop {
public:
	typedef std::map< int, watch > map_t;

	class poll_set {
	public:
		poll_set(map_t const& watches)
			:
				nfds_(watches.size()),
				pfds_((struct pollfd *) malloc(nfds_ * sizeof(struct pollfd)))
		{
			if (!pfds_) {
				throw std::bad_alloc();
			}

			map_t::const_iterator it(watches.begin());
			for (size_t i(0); it != watches.end(); ++it, ++i) {
				pfds_[i] = it->second.pollfd();
			}
		}

		~poll_set()
		{
			free(pfds_);
		}

		size_t nfds() const
		{ return nfds_; }

		struct pollfd *pfds() const
		{ return pfds_; }

		short revents(size_t i) const
		{
			if (i > nfds_) {
				throw std::runtime_error("out of range");
			}

			return pfds_[i].revents;
		}

		int fd(size_t i) const
		{
			if (i > nfds_) {
				throw std::runtime_error("out of range");
			}

			return pfds_[i].fd;
		}

	private:
		poll_set(poll_set const&);
		poll_set & operator=(poll_set const&);

		nfds_t nfds_;
		struct pollfd *pfds_;
	};

	loop() :
		wakeup_fd_(-1),
		watches_(),
		stop_flag_(true)
	{
		int fds[2];

		pipe(fds);
		fcntl(fds[0], F_SETFL, FD_CLOEXEC|O_NONBLOCK);
		fcntl(fds[1], F_SETFL, FD_CLOEXEC|O_NONBLOCK);

		wakeup_fd_ = fds[1];
		add_watch(fds[0], POLLIN, boost::bind(&loop::set_stop_flag, this));
	}


	void run()
	{
		stop_flag_ = false;

		do {
			poll_set pset(watches_);

			int ret(-1);
			do {
				ret = poll(pset.pfds(), pset.nfds(), -1);
			} while (ret == -1 && errno == EINTR);

			std::vector < boost::function<void(void)> > cb;
			for (size_t i(0); i < pset.nfds(); ++i) {
				if (pset.revents(i)) {
					cb.push_back(boost::bind(
						watches_[pset.fd(i)].cb, pset.revents(i)));
				}
			}

			std::vector < boost::function<void(void)> >::iterator cb_it(cb.begin());
			for (; cb_it != cb.end(); ++cb_it) {
				(*cb_it)();
			}

		} while (!stop_flag_);
	}

	void add_watch(int fd, short events, boost::function<void(short)> const& cb)
	{
		watches_[fd] = watch(fd, events, cb);
	}

	void remove_watch(int fd)
	{
		watches_.erase(fd);
	}

	void stop()
	{
		char x(42);
		write(wakeup_fd_, &x, 1);
	}
private:
	void set_stop_flag()
	{
		stop_flag_ = true;
	}

	int wakeup_fd_;
	std::map< int, watch > watches_;
	bool stop_flag_;
};

class editor : public elcc::editor {
public:
	editor(std::string const& name, loop & loop)
		:
			elcc::editor(name, boost::bind(&editor::toggle_watch, this, _1, _2)),
			loop_(loop)
	{
	}

	~editor() {}
private:
	void on_watch_event(short revents)
	{
		if (revents & POLLIN) {
			handle_io();
		}
	}

	void toggle_watch(int fd, bool on)
	{
		if (!on) {
			loop_.remove_watch(fd);
			loop_.stop();
			return;
		}

		loop_.add_watch(fd, POLLIN, boost::bind(&editor::on_watch_event, this, _1));
	}

	loop & loop_;
};

}

void on_line(elcc::editor & el, std::string const& line)
{
	el.history().enter(line);
}

std::string fancy_prompt()
{
	return "ok> ";
}

elcc::function_return eof_handler(dumb_ev::loop & loop)
{
	loop.stop();
	fprintf(stdout, "\n");
	return elcc::eof;
}

elcc::word_list completion_handler(elcc::word_list const&, size_t idx)
{
	std::vector<std::string> cmds;
	switch (idx) {
	case 0:
		cmds.push_back("cmd_a");
		cmds.push_back("cmd_b");
		cmds.push_back("cmd_c");
		break;
	case 1:
		cmds.push_back("arg0_a");
		cmds.push_back("arg0_b");
		cmds.push_back("arg0_c");
		break;
	case 2:
		cmds.push_back("arg1_a");
		cmds.push_back("arg1_b");
		cmds.push_back("arg1_c");
		break;
	default:
		break;
	}

	return cmds;
}

int main()
{
	setlocale(LC_CTYPE, "");

	dumb_ev::loop loop;
	dumb_ev::editor el("elcc", loop);

	el.prompt_cb(&fancy_prompt);
	el.line_cb(boost::bind(&on_line, boost::ref(el), _1));

	el.add_function("exit", "exit at eof", boost::bind(&eof_handler, boost::ref(loop)));
	el.bind("^D", "exit");

	el.bind_completer("^I", boost::bind(&completion_handler, _1, _2));
	el.run();

	loop.run();
	return 0;
}
