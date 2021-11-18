#include "options.hpp"
#include "boost/format.hpp"
#include <iostream>

void announce_options()
{
  std::cout << boost::format(
                   "option name %1% type %2% default %3% min %4% max %5%") %
                   "Hash" % "spin" % "1" % "1" % "128"
            << std::endl;
}
