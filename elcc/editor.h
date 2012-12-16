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

#ifndef EL_CC_EDITOR_H
#define EL_CC_EDITOR_H

#include <string>
#include <boost/function.hpp>
#include <tscb/reactor>

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

namespace impl {
class editor;
}

typedef boost::function<std::string(void)> prompt_function;
typedef boost::function<bool(std::string)> line_function;

// custom editor function, argument is the key entered
typedef boost::function<function_return(int)> editor_function;

class editor {
public:

	editor(std::string const& argv0, tscb::posix_reactor_service & reactor);
	~editor();

	// set the prompt
	void prompt(std::string const&);

	// set a callback for the prompt
	void prompt_cb(prompt_function const& prompt);

	// set a callback for each line
	void line_cb(line_function const& line);

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

	// run the editor
	// this will set the terminal to raw mode
	void run();

private:
	editor(editor const&);
	editor & operator=(editor const&);

	impl::editor *impl_;
};

}

#endif
