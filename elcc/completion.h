#ifndef ELCC_COMPLETION_H
#define ELCC_COMPLETION_H

#include <elcc/common.h>

namespace elcc {

// build completion list
// returns the subset of strings of which string is a prefix
// if more than one result is found the longest common prefix
// is appended to the result
word_list complete(std::string const&, word_list const&);

}

#endif
