#ifndef ENVIRONMENT_VALUES_HPP
# define ENVIRONMENT_VALUES_HPP
# include <iostream>
# include <string>
# include <map>

using namespace std;

class EnvironmentValues
{
    private:
        map<string, string> envList;
    public:
        string getValue(const string &key) const;
        void setPair(const string &key, const string &value);
};

#endif