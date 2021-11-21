#include "util.hpp"

char getc(int i, std::smatch &matches)
{
    if (matches[i].length())
    {
        return matches[i].str().at(0);
    }
    return 0;
}