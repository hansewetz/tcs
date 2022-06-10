#pragma once
#include <boost/program_options.hpp>
#include <optional>
#include <string>
#include <iosfwd>
namespace po=boost::program_options;

namespace tcs{
// print usage info from boost::program_options::positional_options_description
// (and exit)
void cmdusage(boost::program_options::options_description const&desc,boost::program_options::positional_options_description const&posdesc,
           std::string const&progname);
void cmdusage(boost::program_options::options_description const&desc,boost::program_options::positional_options_description const&posdesc,
           std::string const&progname,std::optional<std::string>const&subcmd,std::optional<std::string>const&manpage);
}
