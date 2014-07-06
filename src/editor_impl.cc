/*
 * Copyright (c) 2012, 2013, 2014 Andreas Fett.
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

#include <errno.h>
#include <fcntl.h>

#include <sstream>
#include <stdexcept>

#include <boost/assert.hpp>
#include <boost/bind.hpp>

#include <editor_impl.h>

namespace {

elcc::impl::editor * get_editor(EditLine *el)
{
	void *data(0);
	el_get(el, EL_CLIENTDATA, &data);
	return reinterpret_cast<elcc::impl::editor *>(data);
}

template <int N>
unsigned char function_wrapper(EditLine *el, int c)
{
	elcc::impl::editor *editor(get_editor(el));
	switch (editor->call(N, c)) {
	case elcc::normal:       return CC_NORM;
	case elcc::newline:      return CC_NEWLINE;
	case elcc::eof:          return CC_EOF;
	case elcc::arghack:      return CC_ARGHACK;
	case elcc::refresh:      return CC_REFRESH;
	case elcc::cursor:       return CC_CURSOR;
	case elcc::error:        return CC_ERROR;
	case elcc::fatal:        return CC_FATAL;
	case elcc::redisplay:    return CC_REDISPLAY;
	case elcc::refresh_beep: return CC_REFRESH_BEEP;
	}
	return CC_FATAL;
}

typedef unsigned char (*wrapper_function)(EditLine *, int);

wrapper_function get_wrapper(size_t n)
{
#define CASE_N(index) case index: return &function_wrapper<index>;
	switch (n) {
	CASE_N(0x00); CASE_N(0x01); CASE_N(0x02); CASE_N(0x03);
	CASE_N(0x04); CASE_N(0x05); CASE_N(0x06); CASE_N(0x07);
	CASE_N(0x08); CASE_N(0x09); CASE_N(0x0a); CASE_N(0x0b);
	CASE_N(0x0c); CASE_N(0x0d); CASE_N(0x0e); CASE_N(0x0f);

	CASE_N(0x10); CASE_N(0x11); CASE_N(0x12); CASE_N(0x13);
	CASE_N(0x14); CASE_N(0x15); CASE_N(0x16); CASE_N(0x17);
	CASE_N(0x18); CASE_N(0x19); CASE_N(0x1a); CASE_N(0x1b);
	CASE_N(0x1c); CASE_N(0x1d); CASE_N(0x1e); CASE_N(0x1f);
	}
#undef CASE_N
	return 0;
}

std::string errno_string(std::string const& msg)
{
	std::stringstream out;
	out << msg << ": " << strerror(errno);
	return out.str();
}

} // namespace

namespace elcc {
namespace impl {


editor::editor(std::string const& argv0, watch_function const& watch)
:
	watch_(watch),
	el_(el_init(argv0.c_str(), stdin, stdout, stderr)),
	custom_prompt_(),
	internal_prompt_(),
	on_line_(),
	on_tokenized_line_(),
	history_(INT_MAX),
	fn_index_(0),
	functions_(),
	running_(false),
	fd_(-1),
	tokenizer_()
{
	if (!el_) {
		throw std::runtime_error("failed to initialize editline");
	}

	el_set(el_, EL_CLIENTDATA, this);
	el_set(el_, EL_EDITOR, "emacs");
	el_set(el_, EL_HIST, &::history, history_.get());
}

void editor::run()
{
	if (running_) {
		return;
	}

	el_set(el_, EL_TERMINAL, NULL);
	el_set(el_, EL_PREP_TERM, 1);
	el_set(el_, EL_UNBUFFERED, 1);

	fd_ = fileno(stdin);

	int flags=fcntl(fd_, F_GETFL);
	if (flags == -1) {
		throw std::runtime_error(
			errno_string("fcntl(F_GETFL)"));
	}

	flags |= O_NONBLOCK;
	if (fcntl(fd_, F_SETFL, flags) == -1) {
		throw std::runtime_error(
			errno_string("fcntl(F_SETFL)"));
	}

	watch_(fd_, true);

	running_ = true;
}

function_return editor::call(size_t n, int c) const
{
	return functions_[n].fn(c);
}

const char * editor::internal_prompt_cb(EditLine *el)
{
	return get_editor(el)->internal_prompt_.c_str();
}

const char * editor::custom_prompt_cb(EditLine *el)
{
	return get_editor(el)->custom_prompt_().c_str();
}

void editor::add_function(std::string const& name, std::string const& descr, editor_function const& cb)
{
	wrapper_function fn(get_wrapper(fn_index_));
	if (fn) {
		functions_[fn_index_].name = name;
		functions_[fn_index_].descr = descr;
		functions_[fn_index_].fn = cb;
		el_set(el_, EL_ADDFN, functions_[fn_index_].name.c_str(),
			functions_[fn_index_].descr.c_str(), fn);
		++fn_index_;
	}
}

void editor::bind(std::string const& key, std::string const& name)
{
	el_set(el_, EL_BIND, key.c_str(), name.c_str(), NULL);
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

void editor::unwatch()
{
	if (watch_) {
		watch_(fd_, false);
		watch_ = 0;
	}
}

editor::~editor()
{
	unwatch();

	el_set(el_, EL_PREP_TERM, 0);

	if (el_) {
		el_end(el_);
	}
}

void editor::handle_io()
{
	int count(0);

	errno = 0;
	const char *line(el_gets(el_, &count));
	if (!line || count < 1) {
		if (count < 0 && (errno == EAGAIN || errno == EINTR)) {
			return;
		}

		if (errno == 0) {
			// happens when backspace aka vi-yank is
			// encountered at the beginning of the line
			return;
		}

		unwatch();
		return;
	}

	if (line[count - 1] == '\n') {
		el_set(el_, EL_UNBUFFERED, 0);

		if (on_line_) {
			on_line_(std::string(line, count - 1));
		}

		if (on_tokenized_line_) {
			on_tokenized_line_(tokenized_line().line);
		}

		el_set(el_, EL_UNBUFFERED, 1);
	}
}

std::string editor::line() const
{
	LineInfo const* li(el_line(el_));
	return std::string(li->buffer, li->lastchar - li->buffer);
}

std::string editor::cursor_line() const
{
	LineInfo const* li(el_line(el_));
	return std::string(li->buffer, li->cursor - li->buffer);
}

token_line editor::tokenized_line()
{
	LineInfo const* li(el_line(el_));
	return tokenizer_(li);
}

size_t editor::cursor() const
{
	LineInfo const* li(el_line(el_));
	return li->cursor - li->buffer;
}

void editor::insert(std::string const& str)
{
	el_insertstr(el_, str.c_str());
}

}} // namespace elcc::impl
