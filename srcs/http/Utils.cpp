#include "Utils.hpp"

Utils::Utils() {}

Utils::~Utils() {}

vector<string> Utils::split(const string &s, const string &delim)
{
	vector<string> result;
	size_t start = 0;
	size_t end = s.find(delim);
	while (end != string::npos)
	{
		result.push_back(s.substr(start, end - start));
		start = end + delim.size();
		end = s.find_first_of(delim, start);
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