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

#include <stdexcept>

#include <tokenizer.h>
namespace elcc {

tokenizer::tokenizer(std::string const& ifs)
:
	tokenizer_(tok_init(ifs.empty() ? NULL : ifs.c_str()))
{
	if (!tokenizer_) {
		throw std::bad_alloc();
	}
}

void tokenizer::reset()
{ tok_reset(tokenizer_); }

token_line tokenizer::operator()(const LineInfo *li)
{
	const char **argv;
	int argc, cursorc, cursoro;

	int ret(tok_line(tokenizer_, li, &argc, &argv, &cursorc, &cursoro));
	if (ret != 0) {
		reset();
		throw std::runtime_error("failed to parse line");
	}

	token_line res;
	for (int i(0); i < argc; ++i) {
		res.line.push_back(argv[i]);
	}

	res.cursor_word = cursorc;
	res.cursor_offset = cursoro;

	reset();
	return res;
}

tokenizer::~tokenizer()
{
	tok_end(tokenizer_);
}

}
