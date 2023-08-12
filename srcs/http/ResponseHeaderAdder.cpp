#include "ResponseHeaderAdder.hpp"
ResponseHeaderAdder::ResponseHeaderAdder(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessage, \
    const LocationConfig & locationConfig, const string & responseBody, const string & resourcePath, const string & contentType)
    : requestMessage(requestMessage), responseMessage(responseMessage), locationConfig(locationConfig), \
        responseBody(responseBody), resourcePath(resourcePath), contentType(contentType) {}


void ResponseHeaderAdder::executeAll()
{
    const int statusCode = responseMessage.getStatusCode();
    
    if (statusCode == 201) { // 리소스 생성 성공
        addLocationHeader(resourcePath); 
    }
    else if (statusCode == 405) { // 허락하지 않은 메소드를 받음
        addAllowHeader(locationConfig.getAcceptMethods());
    }
    addContentTypeHeader(contentType);
    addContentLengthHeader(responseBody);
    addDateHeader();
    if (requestMessage.getHeader("Connection") == "close") {
        addConnectionHeader(false);
    }
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

void ResponseHeaderAdder::parseCgiProduct(string & response, string & contentType)
{
    size_t newline = response.find("\n\n");
    if (newline != string::npos) {
        string header = response.substr(0, newline);
        if (header.find(":") != string::npos) {
            if (header.substr(0, 12) == "Content-type") {
                contentType = Utils::ltrim(header.substr(13, header.length()-13));
                response = response.substr(newline+2, response.length()-newline-2);
            }
        }
    }
}