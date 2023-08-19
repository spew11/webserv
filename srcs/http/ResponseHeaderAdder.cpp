#include "ResponseHeaderAdder.hpp"

// ResponseHeaderAdder::ResponseHeaderAdder(HttpResponseMessage & responseMessage)
// {

// }

ResponseHeaderAdder::ResponseHeaderAdder(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessage, \
    const LocationConfig & locationConfig, const string & resourcePath)
    : requestMessage(requestMessage), responseMessage(responseMessage), locationConfig(locationConfig), resourcePath(resourcePath) {}


void ResponseHeaderAdder::executeAll()
{
    cout << "ReposneHeaderAdder::executeAll() 시작" << endl;
    const int statusCode = responseMessage.getStatusCode();
    
    if (statusCode == 201) { // 리소스 생성 성공
        addLocationHeader(resourcePath); 
    }
    else if (statusCode == 405) { // 허락하지 않은 메소드를 받음
        addAllowHeader(locationConfig.getAcceptMethods());
    }
    addContentTypeHeader(locationConfig.getType(resourcePath));
    addContentLengthHeader(responseMessage.getBody());
    addDateHeader();
    if (requestMessage.getHeader("Connection") == "close") {
        addConnectionHeader(false);
    }
    cout << "ReposneHeaderAdder::executeAll() 끝" << endl;
}

void ResponseHeaderAdder::addContentTypeHeader(const string & contentType)
{
    responseMessage.addHeader("Content-Type", contentType);
}

void ResponseHeaderAdder::addContentLengthHeader(const string & responseBody)
{
    responseMessage.addHeader("Content-Length", Utils::itoa(responseBody.length()));
}


void ResponseHeaderAdder::addLocationHeader(const string & location)
{
    responseMessage.addHeader("Location", location);
}

void ResponseHeaderAdder::addAllowHeader(const vector<string> & acceptMethods)
{
    string methods;
    for (int i = 0; i < acceptMethods.size()-1; i++) {
        methods += acceptMethods.at(i) + ", ";
    }
    methods += acceptMethods.at(acceptMethods.size()-1);
    responseMessage.addHeader("Allow", methods);
}

void ResponseHeaderAdder::addDateHeader()
{
    time_t now = time(0);
    tm *gmt = gmtime(&now);
    char buffer[100];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S %Z", gmt);
    responseMessage.addHeader("Date", string(buffer));
}

void ResponseHeaderAdder::addConnectionHeader(const bool & connect)
{
    if (connect == false) {
        responseMessage.addHeader("Connection", "close");
    }
    else {
        responseMessage.addHeader("Connection", "keep-alive");
    }
}

