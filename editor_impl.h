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

#ifndef EL_CC_EDITOR_IMPL_H
#define EL_CC_EDITOR_IMPL_H

#include <stddef.h>

#include <histedit.h>

#include <elcc/editor.h>

namespace elcc {
namespace impl {

class history {
public:
	history(size_t size);
	History *get() const;
	void add(std::string const& line);
	~history();

private:
	History *history_;
	HistEvent event_;
};

class editor {
public:
	editor(std::string const&, tscb::posix_reactor_service &);
	void prompt(std::string const&);
	void prompt_cb(prompt_function const&);
	void line_cb(line_function const&);
	void run();
	~editor();

private:
	static editor * self(EditLine *);
	static const char * custom_prompt_cb(EditLine *);
	static const char * internal_prompt_cb(EditLine *);
	void on_readable(int);

	tscb::posix_reactor_service & reactor_;
	EditLine *el_;
	prompt_function custom_prompt_;
	std::string internal_prompt_;
	line_function on_line_;
	tscb::connection stdin_;
	history history_;
	bool running_;
};

}}

#endif
