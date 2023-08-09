#ifndef WEBSERV_VALUES_HPP
# define WEBSERV_VALUES_HPP
# include <iostream>
# include <string>
# include <map>
# include <sstream>

using namespace std;

class WebservValues
{
    private:
        map<string, string> envList;
    public:
        string getValue(const string &key) const;
        string convert(const string &input) const;
        void insert(const string &key, const string &value);
        void insert(const string &key, const uint16_t &value);
        void clear();
};

#endif
