# include "Utils.hpp"

vector<string> Utils::split(const string& s, const string& delim)
{
    vector<string> result;
    size_t start = 0;
    size_t end = s.find_first_of(delim);
    while (end != string::npos) {
        result.push_back(s.substr(start, end - start));
        start = end + 1;
        end = s.find_first_of(delim, start);
        if (end == string::npos) {
            result.push_back(s.substr(start, s.length()));
            break;
        }
    }
    return result;
}

string Utils::ltrim(const string &s)
{
    string newString(s);
    newString.erase(0, newString.find_first_not_of(" \t\n\r\f\v"));
    return newString;
}