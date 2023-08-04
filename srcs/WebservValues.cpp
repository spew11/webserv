#include "WebservValues.hpp"

string WebservValues::getValue(const string &key) const
{
    if (envList.find(key) != envList.end()) {
        return envList.at(key);
    }
    return "";
}

void WebservValues::insert(const string &key, const string &value)
{
    envList.insert(pair<string, string>(key, value));
}
