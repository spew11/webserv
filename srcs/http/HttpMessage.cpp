#include "HttpMessage.hpp"

string HttpMessage::getBody() const
{
	return body;
}

string HttpMessage::getServerProtocol() const
{
	return serverProtocol;
}

map<string, string> HttpMessage::getHeaders() const
{
	return headers;
}

string HttpMessage::getHeader(const string &headerType) const
{
	if (headers.find(headerType) != headers.end())
	{
		return headers.at(headerType);
	}
	return "";
}

void HttpMessage::printHeaders() const
{
	map<string, string>::const_iterator it;
	for (it = headers.begin(); it != headers.end(); it++)
	{
		cout << "key: " << it->first << " ***** value: " << it->second << endl;
	}
}