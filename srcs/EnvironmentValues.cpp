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

string EnvironmentValues::replace(const string &input) const
{
    string result = input;
    size_t pos = 0;

    while ((pos = result.find('$', pos)) != string::npos)
    {
        size_t endPos = result.find('$', pos + 1);

        if (endPos == string::npos)
            endPos = result.back();

        string varName = result.substr(pos + 1, endPos - pos - 1);
        string varValue = getValue(varName);
        
        result.replace(pos, endPos - pos, varValue);
        pos = pos + varValue.length();
    }

    return result;
}
