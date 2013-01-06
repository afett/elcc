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

#include <boost/bind.hpp>

#include <tscb/reactor>

#include <elcc/tscb/editor.h>

namespace elcc {
namespace tscb {

editor::editor(std::string const& name, ::tscb::posix_reactor_service & reactor)
:
	elcc::editor(name, boost::bind(&editor::toggle_watch, this, _1, _2)),
	reactor_(reactor),
	conn_()
{ }

void editor::toggle_watch(int fd, bool on)
{
	if (!on && conn_.connected()) {
		conn_.disconnect();
		return;
	}

	conn_ = reactor_.watch(boost::bind(&editor::on_ioready, this, _1),
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
