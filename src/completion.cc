#include <algorithm>

#include <boost/bind.hpp>

#include <elcc/completion.h>

namespace {

bool begins_not_with(std::string const& s1, std::string const& s2)
{
	return s2.compare(0, s1.size(), s1) != 0;
}

std::string find_prefix(elcc::word_list const& strings)
{
	elcc::word_list::const_iterator it(strings.begin());
	std::string prefix(*it++);

	for (; it != strings.end(); ++it) {
		while (begins_not_with(prefix, *it)) {
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
	// sigh ...
	std::remove_copy_if(strings.begin(), strings.end(),
		std::back_inserter(res), boost::bind(begins_not_with, string, _1));

	if (res.size() > 1) {
		res.push_back(find_prefix(res));
	}

	return res;
}

}


