#ifndef RESPONSE_HEADER_ADDER_HPP
#define RESPONSE_HEADER_ADDER_HPP
#include <iostream>
#include <string>
#include "HttpRequestMessage.hpp"
#include "HttpResponseMessage.hpp"
#include "ResponseHeaderAdder.hpp"
#include "ServerConfig.hpp"
#include "HttpResponseBuilder.hpp"
#include "Utils.hpp"

using namespace std;

class HttpResponseBuilder;

class ResponseHeaderAdder
{
public:
	void executeAll(HttpResponseBuilder &reponseBuilder);
	void addContentTypeHeader(HttpResponseMessage &responseMessage, const string &contentType);
	void addContentLengthHeader(HttpResponseMessage &responseMessage, const string &responseBody);
	void addLocationHeader(HttpResponseMessage &responseMessage, const string &location);
	void addAllowHeader(HttpResponseMessage &responseMessage, const vector<string> &acceptMethods);
	void addConnectionHeader(HttpResponseMessage &responseMessage, const bool &connect);
	void addDateHeader(HttpResponseMessage &responseMessage);
};

#endif