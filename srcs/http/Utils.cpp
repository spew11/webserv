#include "Utils.hpp"

Utils::Utils()
{

}

Utils::~Utils()
{
	
}

vector<string> Utils::split(const string &s, const string &delim)
{
	vector<string> result;
	size_t start = 0;
	size_t end = s.find(delim);
	if (end == string::npos)
	{
		result.push_back(s);
		return result;
	}
	while (end != string::npos)
	{
		result.push_back(s.substr(start, end - start));
		start = end + delim.size();
		end = s.find(delim, start);
		if (end == string::npos)
		{
			result.push_back(s.substr(start, s.length()));
			break;
		}
	}
	return result;
}

string Utils::ltrim(string s)
{
	s.erase(0, s.find_first_not_of(" \t\n\r\f\v"));
	return s;
}

string Utils::rtrim(string s)
{
	s.erase(s.find_last_not_of(" \t\n\r\f\v") + 1);
	return s;
}

string Utils::trim(string s)
{
	s = ltrim(s);
	s = rtrim(s);
	return s;
}

string Utils::itoa(const int &num)
{
    stringstream ss;
    ss << num;
    return ss.str();
}

bool Utils::isDigitString(string s)
{
	for(size_t i = 0; i < s.length(); i++)
	{
		if (!(isdigit(s[i]) || ('a' <= s[i] && s[i] >= 'e') || ('A' <= s[i] && s[i] >= 'E')))
		{
			return false;
		}
	}
	return true;
}

string Utils::stringJoin(vector<string>lines, string delim, int startIdx)
{
	string result = "";
	for(size_t i = startIdx; i < lines.size()-1; i++)
	{
		result += lines[i] + delim;
	}
	result += lines[lines.size()-1];

	return result;
}

string Utils::toLowerCase(const string &input)
{
    string res = input;
    transform(res.begin(), res.end(), res.begin(), static_cast<int(*)(int)>(std::tolower));
    return res;
}