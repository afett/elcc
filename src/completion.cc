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

#include <algorithm>
#include <functional>

#include <elcc/completion.h>

namespace {

bool begins_with(std::string const& s1, std::string const& s2)
{
	return s2.compare(0, s1.size(), s1) == 0;
}

std::string find_prefix(elcc::word_list const& strings)
{
	auto it(strings.begin());
	auto prefix(*it++);

	for (; it != strings.end(); ++it) {
		while (!begins_with(prefix, *it)) {
			prefix.resize(prefix.size() - 1);
		}
	}

	return prefix;
}

}

namespace elcc {

word_list complete(std::string const& string, word_list const& strings)
{
	word_list res;
	std::copy_if(strings.begin(), strings.end(),
		std::back_inserter(res), std::bind(begins_with, string, std::placeholders::_1));

	if (res.size() > 1) {
		res.push_back(find_prefix(res));
	}

	return res;
}

}
