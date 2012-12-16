/*
 * Copyright (c) 2012 Andreas Fett.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <fcntl.h>

#include <stdexcept>

#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>

#include <editor_impl.h>

namespace elcc {
namespace impl {

history::history(size_t size)
:
	history_(history_init())
{
	BOOST_ASSERT(history_);
	 ::history(history_, &event_, H_SETSIZE, size);
	 ::history(history_, &event_, H_SETUNIQUE, 1);
}

History *history::get() const
{ return history_; }

void history::add(std::string const& line)
{ ::history(history_, &event_, H_ENTER, line.c_str()); }

history::~history()
{ history_end(history_); }

editor::editor(std::string const& argv0, tscb::posix_reactor_service & reactor)
:
	reactor_(reactor),
	el_(el_init(argv0.c_str(), stdin, stdout, stderr)),
	history_(INT_MAX),
	running_(false)
{
	if (!el_) {
		throw std::runtime_error("failed to initialize editline");
	}

	el_set(el_, EL_CLIENTDATA, this);
	el_set(el_, EL_EDITOR, "emacs");
	el_set(el_, EL_ADDFN, "elcc-eof", "end of file", &editor::internal_eof_cb);
	el_set(el_, EL_HIST, &::history, history_.get());
}

void editor::run()
{
	BOOST_ASSERT(!running_);

	el_set(el_, EL_UNBUFFERED, 1);
	el_set(el_, EL_TERMINAL, NULL);
	el_set(el_, EL_PREP_TERM, 1);

	int fd(fileno(stdin));

	int flags=fcntl(fd, F_GETFL);
	if (flags == -1) {
		throw std::runtime_error(boost::str(
			boost::format("fcntl(%i, F_GETFL): %s")
				% fd % strerror(errno)));
	}

	flags |= O_NONBLOCK;
	if (fcntl(fileno(stdin), F_SETFL, flags) == -1) {
		throw std::runtime_error(boost::str(
			boost::format("fcntl(%i, F_SETFL): %s")
				% fd % strerror(errno)));
	}

	stdin_ = reactor_.watch(
		boost::bind(&editor::on_readable, this, _1),
		fd, tscb::ioready_input);

	running_ = true;
}

const char * editor::internal_prompt_cb(EditLine *el)
{
	return elcc::impl::editor::self(el)->internal_prompt_.c_str();
}

const char * editor::custom_prompt_cb(EditLine *el)
{
	return elcc::impl::editor::self(el)->custom_prompt_().c_str();
}

unsigned char editor::internal_eof_cb(EditLine *el, int)
{
	elcc::impl::editor::self(el)->on_eof_();
	return CC_EOF;
}

void editor::prompt_cb(prompt_function const& prompt)
{
	custom_prompt_ = prompt;
	el_set(el_, EL_PROMPT, &editor::custom_prompt_cb);
}

void editor::prompt(std::string const& prompt)
{
	internal_prompt_ = prompt;
	el_set(el_, EL_PROMPT, &editor::internal_prompt_cb);
}

void editor::line_cb(line_function const& cb)
{ on_line_ = cb; }

void editor::eof_cb(eof_function const& cb)
{
	on_eof_ = cb;
	el_set(el_, EL_BIND, "^D", "elcc-eof", NULL);
}

editor::~editor()
{
	if (stdin_.connected()) {
		stdin_.disconnect();
	}

	el_set(el_, EL_PREP_TERM, 0);

	if (el_) {
		el_end(el_);
	}
}

editor * editor::self(EditLine *el)
{
	void *data(0);
	el_get(el, EL_CLIENTDATA, &data);
	return reinterpret_cast<editor *>(data);
}

void editor::on_readable(int event)
{
	int count(0);
	const char *line(el_gets(el_, &count));
	if (!line) {
		return;
	}

	if (line[count - 1] == '\n') {
		el_set(el_, EL_UNBUFFERED, 0);
		if (count > 1 && on_line_(std::string(line, count -1))) {
			history_.add(line);
		}
		el_set(el_, EL_UNBUFFERED, 1);
	}
}

}}
