#include "HttpRequestMessage.hpp"
#include <cctype>
#include <algorithm>
#include <fstream>
#include <stdlib.h>

HttpRequestMessage::HttpRequestMessage(const string httpMethod, const string requestTarget, string serverProtocol, map<string, string> headers, string body)
{
    this->httpMethod = httpMethod;
    this->requestTarget = requestTarget;
    this->serverProtocol = serverProtocol;
    this->headers = headers;
    this->body = body;
    chunked = false;
    connection = true;
    errorCode = 500;
    setFlag();
}

void HttpRequestMessage::setFlag()
{
    map<string, string>::iterator it = headers.begin();

    for (; it != headers.end(); it++) 
    {
        string headerType = Utils::toLowerCase(it->first);
        string headerValue = it->second;
        if (headerType == "tansfer-encoding" && headerValue == "chunked") {
            chunked = true;
        }
        else if (headerType == "connection" && headerValue == "close") {
            connection = false;
        }
    }
}

string HttpRequestMessage::getHttpMethod() const
{
	return httpMethod;
}

string HttpRequestMessage::getRequestTarget() const
{
	return requestTarget;
}

bool HttpRequestMessage::getChunked() const
{
	return chunked;
}

bool HttpRequestMessage::getConnection() const
{
	return connection;
}

int HttpRequestMessage::getErrorCode() const
{
	return errorCode;
}
