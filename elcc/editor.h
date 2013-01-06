/*
 * Copyright (c) 2012, 2013 Andreas Fett.
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

#ifndef EL_CC_EDITOR_H
#define EL_CC_EDITOR_H

#include <boost/function.hpp>

#include <elcc/common.h>

namespace elcc {

// return codes for custom editor functions
enum function_return {
	normal,       // Add a normal character.
	newline,      // End of line was entered.
	eof,          // EOF was entered.
	arghack,      // Expecting further command input as arguments,
	              //  do nothing visually.
	refresh,      // Refresh display.
	cursor,       // Cursor moved, so update and perform refresh.
	error,        // An error occurred.  Beep, and flush tty.
	fatal,        // Fatal error, reset tty to known state.
	redisplay,    // Redisplay entire input line.  This is useful if
	              //  a key binding outputs extra information.
	refresh_beep, // Refresh display, and beep.
};

class editor;
class history;

namespace impl {
class editor;
}

typedef boost::function<std::string(void)> prompt_function;
typedef boost::function<void(std::string)> line_function;
typedef boost::function<void(word_list)> tokenized_line_function;

// (un)watch a filedescriptor
// @arg fd filedescriptor
// @on (un)watch it
typedef boost::function<void(int, bool)> watch_function;

// custom editor function, argument is the key entered
typedef boost::function<function_return(int)> editor_function;

struct token_line {
	word_list line;
	size_t cursor_word;
	size_t cursor_offset;
};

// callback for the internal auto completer
// must return a vector of words to which the current word
// could be completed.
typedef boost::function<word_list(word_list, size_t)> completion_function;

class editor {
public:
	// constructor
	// @arg name name of the editor
	// @arg watch toggle watch of a filedescriptor
	editor(std::string const&, watch_function const&);
	~editor();

	// call this function to signal that the filedescriptor
	// that was passed to the watch function is readable
	void handle_io();

	// set the prompt
	void prompt(std::string const&);

	// set a callback for the prompt
	void prompt_cb(prompt_function const& prompt);

	// set a callback for each line, the trailing
	// newline is stripped.
	void line_cb(line_function const& line);

	// set a callback for each line, the line is broken into tokens
	// if both line callbacks are set, both will be called
	void tokenized_line_cb(tokenized_line_function const& line);

	// add a user defined editor function
	// a maximum of 32 functions may be defined
	// @arg name name of the function
	// @arg descr descrition of the function
	// @arg function function to be called
	void add_function(std::string const&, std::string const&, editor_function const&);

	// bind a key to a function
	// @arg key name of the key "^A" for example
	// @arg function name of the custom or buildin function
	void bind(std::string const&, std::string const&);

	// install the internal auto completer
	// @arg key key to which the completer is bound
	// @arg completion_function function to determine the completions
	void bind_completer(std::string const&, completion_function const&);

	// get the history object
	elcc::history & history() const;

	std::string line() const;
	std::string cursor_line() const;
	token_line tokenized_line();
	size_t cursor() const;
	void insert(std::string const&);

	// detect consecutive input of the
	// same character as in TAB TAB for completion
	// to be used from within an editor function
	// the counter is reset by handle_io()
	size_t key_count() const;
	void count_key();

	// run the editor
	// this will set the terminal to raw mode
	void run();

private:
	editor(editor const&);
	editor & operator=(editor const&);

	impl::editor *impl_;
	size_t key_count_;
};

}

#endif
