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

#include <history_impl.h>

namespace elcc {

history::~history()
{ }

namespace impl {

history::history(size_t size)
:
	history_(history_init())
{
	if (!history_) {
		throw std::bad_alloc();
	}

	 ::history(history_, &event_, H_SETSIZE, size);
	 ::history(history_, &event_, H_SETUNIQUE, 1);
}

History *history::get() const
{ return history_; }


history::~history()
{ history_end(history_); }

size_t history::size()
{
	int size(0);
	::history(history_, &event_, H_GETSIZE, &size);
	return size;
}

void history::clear()
{ ::history(history_, &event_, H_CLEAR); }

void history::add(std::string const& line)
{ ::history(history_, &event_, H_ADD, line.c_str()); }

void history::append(std::string const& line)
{ ::history(history_, &event_, H_APPEND, line.c_str()); }

void history::enter(std::string const& line)
{ ::history(history_, &event_, H_ENTER, line.c_str()); }


}}
