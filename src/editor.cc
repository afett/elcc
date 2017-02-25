/*
 * Copyright (c) 2012 - 2015, 2017 Andreas Fett.
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

#include <iostream>

#include <editor_impl.h>
#include <elcc/completion.h>
#include <elcc/editor.h>

namespace {

elcc::function_return completion_handler(elcc::editor * el, elcc::completion_function const& completer)
{
	elcc::token_line tl(el->tokenized_line());
	elcc::word_list words(completer(tl.line, tl.cursor_word));

	std::string word;
	if (tl.cursor_word < tl.line.size()) {
		word = tl.line[tl.cursor_word].substr(0, tl.cursor_offset);
	}

	elcc::word_list completions(elcc::complete(word, words));
	if (completions.empty()) {
		return elcc::refresh_beep;
	}

	std::string completion(completions.back());
	completions.pop_back();
	if (completions.empty()) {
		completion += " ";
	} else {
		if (el->key_count()) {
			std::cout << std::endl;
			for (auto completion: completions) {
				std::cout << completion << std::endl;
			}
		}
		el->count_key();
	}

	el->insert(completion.substr(tl.cursor_offset, std::string::npos));
	return elcc::redisplay;
}

}

namespace elcc {

token_line::token_line()
:
	error(EOK),
	line(),
	cursor_word(0),
	cursor_offset(0)
{ }

editor::editor(std::string const& argv0, watch_function const& watch)
:
	impl_(new impl::editor(argv0, watch)),
	key_count_(0)
{ }

editor::~editor()
{ }

void editor::handle_io()
{
	if (key_count_ > 0) {
		--key_count_;
	}
	impl_->handle_io();
}

size_t editor::key_count() const
{ return key_count_; }

void editor::count_key()
{ key_count_ = 2; }

void editor::prompt(std::string const& prompt)
{ impl_->prompt(prompt); }

void editor::prompt_cb(prompt_function const& cb)
{ impl_->prompt_cb(cb); }

void editor::line_cb(line_function const& cb)
{ impl_->on_line_ = cb; }

void editor::tokenized_line_cb(tokenized_line_function const& cb)
{ impl_->on_tokenized_line_ = cb; }

void editor::add_function(std::string const& name, std::string const& descr, editor_function const& cb)
{ impl_->add_function(name, descr, cb); }

void editor::bind(std::string const& key, std::string const& name)
{ impl_->bind(key, name); }

void editor::bind_completer(std::string const& key, completion_function const& completer)
{
	impl_->add_function("elcc-default-complete", "commandline completion",
		std::bind(&::completion_handler, this, completer));
	impl_->bind(key, "elcc-default-complete");
}

elcc::history & editor::history() const
{ return impl_->history_; }

void editor::start()
{ impl_->start(); }

void editor::enable()
{ impl_->enable(); }

void editor::disable()
{ impl_->disable(); }

void editor::refresh()
{ impl_->refresh(); }

void editor::async_output()
{ impl_->async_output(); }

void editor::async_output_flush()
{ impl_->async_output_flush(); }

std::string editor::line() const
{ return impl_->line(); }

std::string editor::cursor_line() const
{ return impl_->cursor_line(); }

token_line editor::tokenized_line() const
{ return impl_->tokenized_line(); }

size_t editor::cursor() const
{ return impl_->cursor(); }

void editor::insert(std::string const& str)
{ return impl_->insert(str); }

}
