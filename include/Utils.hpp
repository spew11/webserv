#ifndef UTILS_HPP
# define UTILS_HPP
# include <iostream>
# include <string>
# include <vector>
# include <map>
# include <list>
# include <algorithm>

using namespace std;

class Utils 
{
    public:
        vector<string> split(const string& s, const string& delim);
        string ltrim(const string &s);
};

#endif