#include "WebservValues.hpp"

string WebservValues::getValue(const string &key) const
{
	if (envList.find(key) != envList.end())
	{
		return envList.at(key);
	}
	return "";
}

void WebservValues::insert(const string &key, const string &value)
{
	envList[key] = value;
}

void WebservValues::insert(const string &key, const uint16_t &value)
{
	int tmp = static_cast<int>(value);
	stringstream ss;
	ss << tmp;
	envList.insert(pair<string, string>(key, ss.str()));
}

string WebservValues::convert(const string &input) const
{
	string result = input;
	size_t pos = 0;

	while ((pos = result.find('$', pos)) != string::npos)
	{
		size_t endPos = result.find('$', pos + 1);

		if (endPos == string::npos)
			endPos = result.size();

		string varName = result.substr(pos + 1, endPos - pos - 1);
		string varValue = getValue(varName);

		result.replace(pos, endPos - pos, varValue);
		pos = pos + varValue.length();
	}

	return result;
}

void WebservValues::clear()
{
	envList.clear();
	envList = addressValues;
}

void WebservValues::initEnvList()
{
	addressValues = envList;
}