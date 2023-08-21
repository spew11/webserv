#include "ResponseHeaderAdder.hpp"

void ResponseHeaderAdder::executeAll(HttpResponseBuilder &responseBuilder)
{
	HttpResponseMessage &responseMessage = responseBuilder.getResponseMessage();
    const int statusCode = responseBuilder.getResponseMessage().getStatusCode();
    if (statusCode == 201) // 리소스 생성 성공
	{
		addLocationHeader(responseMessage, responseBuilder.getResourcePath());
	}
    else if (statusCode == 405) // 허락하지 않은 메소드
	{
		addAllowHeader(responseMessage, responseBuilder.getLocationConfig().getAcceptMethods());
    }
	else if (statusCode == 301 || statusCode == 302 || statusCode == 303 
			 || statusCode == 307 || statusCode == 308) // 요청한 리소스 이동 됨
	{
		addLocationHeader(responseMessage, responseBuilder.getRedirectUri());
	}
	// defualt headers
	addContentTypeHeader(responseMessage, responseBuilder.getContentType());
	addContentLengthHeader(responseMessage, responseBuilder.getResponseMessage().getBody());
	addConnectionHeader(responseMessage, responseBuilder.getConnection());
    addDateHeader(responseMessage);
}

void ResponseHeaderAdder::addContentTypeHeader(HttpResponseMessage &responseMessage, const string &contentType)
{
	responseMessage.addHeader("Content-Type", contentType);
}

void ResponseHeaderAdder::addContentLengthHeader(HttpResponseMessage &responseMessage, const string &responseBody)
{
	responseMessage.addHeader("Content-Length", Utils::itoa(responseBody.length()));
}

void ResponseHeaderAdder::addLocationHeader(HttpResponseMessage &responseMessage, const string &location)
{
	responseMessage.addHeader("Location", location);
}

void ResponseHeaderAdder::addAllowHeader(HttpResponseMessage &responseMessage, const vector<string> &acceptMethods)
{
	string methods;
	for(size_t i = 0; i < acceptMethods.size() - 1; i++)
	{
		methods += acceptMethods.at(i) + ", ";
	}
	methods += acceptMethods.at(acceptMethods.size() - 1);
	responseMessage.addHeader("Allow", methods);
}

void ResponseHeaderAdder::addDateHeader(HttpResponseMessage &responseMessage)
{
	time_t now = time(0);
	tm *gmt = gmtime(&now);
	char buffer[100];
	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S %Z", gmt);
	responseMessage.addHeader("Date", string(buffer));
}

void ResponseHeaderAdder::addConnectionHeader(HttpResponseMessage &responseMessage, const bool &connect)
{
	if (connect == false)
	{
		responseMessage.addHeader("Connection", "close");
	}
	else
	{
		responseMessage.addHeader("Connection", "keep-alive");
	}
}
