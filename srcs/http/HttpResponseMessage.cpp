#include "HttpResponseMessage.hpp"

HttpResponseMessage::HttpResponseMessage()
{
    this->serverProtocol = "HTTP/1.1";
    cout << "HttpResponseMessage 생성자 호출" << endl;
}

int HttpResponseMessage::getStatusCode() const
{
    return statusCode;
}

string HttpResponseMessage::getReasonPhrase() const
{
    return reasonPhrase;
}

void HttpResponseMessage::setStatusCode(int statusCode)
{
    this->statusCode = statusCode;
}

void HttpResponseMessage::setReasonPhrase(const string &reasonPhrase)
{
    this->reasonPhrase = reasonPhrase;
}

void HttpResponseMessage::setBody(const string &body)
{
    this->body = body;
}

void HttpResponseMessage::addHeader(string headerType, string headerValue)
{
    headers.insert(make_pair(headerType, headerValue));
}

void HttpResponseMessage::setServerProtocol(const string &serverProtocol)
{
    this->serverProtocol = serverProtocol;
}