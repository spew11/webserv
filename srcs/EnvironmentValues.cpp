#include "EnvironmentValues.hpp"

string EnvironmentValues::getValue(const string &key) const
{
    if (envList.find(key) != envList.end()) {
        return envList.at(key);
    }
    return "";
}

void EnvironmentValues::setPair(const string &key, const string &value)
{
    envList.insert(pair<string, string>(key, value));
}