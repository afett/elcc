#ifndef ELCC_COMPLETION_H
#define ELCC_COMPLETION_H

#include <string>
#include <vector>

namespace elcc {

// build completion list
// returns the subset of strings of which string is a prefix
// if more than one result is found the longest common prefix
// is appended to the result
std::vector<std::string> complete(std::string const&, std::vector<std::string> const&);

}

#endif
