#include "HttpRequestMessage.hpp"

HttpRequestMessage::HttpRequestMessage(const string &requestMessage)
{
    chunkedFlag = false;
    parseRequestMessage(requestMessage);

}

// <경로>;<파라미터>?<질의>#<프래그먼트> 경로조각은 없다고 가정
void HttpRequestMessage::parseUri()
{
    // requestUri 초기화
    requestUri = requestTarget;

    // uri 초기화
    size_t pos = requestUri.find_first_of(";?#");
    if (pos == string::npos) {
        uri = requestUri;
    }
    else {
        uri = requestUri.substr(0, pos);
    }

    // filename 초기화
    filename = requestUri.substr(requestUri.find_last_of("/")+1, pos-requestUri.find_last_of("/")-1);

    // args 초기화
    pos = requestUri.find(";");
    if (pos != string::npos) {
        args = requestUri.substr(pos+1, min(requestUri.find("?"), requestUri.length()));
    }

    // queryString 초기화
    pos = requestUri.find("?");
    if (pos != string::npos) {
        queryString = requestUri.substr(pos+1, min(requestUri.find("#"), requestUri.length())-pos-1);
    }
}

void HttpRequestMessage::parseRequestMessage(const string &request)
{
    Utils utils;
    vector<string> lst = utils.split(request, "\r\n");
    
    //start line parsing
    vector<string> tmp = utils.split(lst.at(0), " ");
    httpMethod = tmp.at(0);
    uri = tmp.at(1);
    serverProtocol = tmp.at(2);
    
    int byte = lst.at(0).length()+2; // 나중에 바디 시작 인덱스 알려면 필요
    
    //headers parsing
    int i = 1;
    bool crlf = false;
    for (; i < lst.size(); i++) {
        if (crlf == false && lst.at(i) == "") {
            crlf = true;
            byte += 2;
            break;
        }
        if (lst.at(i).find(":") != string::npos) {
            tmp = utils.split(lst.at(i), ":");
            headers.insert(make_pair(tmp.at(0), utils.ltrim(tmp.at(1))));
            byte += lst.at(i).length() + 2;
        }
    }
    
    //body parsing
    if (byte == request.length()) {
        body = "";
    }
    else if (this->getHeader("Transfer-Encoding") == "chunked") {
        chunkedFlag = static_cast<bool>(stoi(lst.at(i), 0, 16));
        byte += lst.at(i).length();
    }
    else {
        body = request.substr(byte, request.length()-byte);
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

int HttpRequestMessage::getChunkedFlag() const
{
    return chunkedFlag;
}

string HttpRequestMessage::getRequestUri() const
{
    return requestUri;
}

string HttpRequestMessage::getUri() const
{
    return uri;
}

string HttpRequestMessage::getFilename() const
{
    return filename;
}

string HttpRequestMessage::getArgs() const
{
    return args;
}

string HttpRequestMessage::getQueryString() const
{
    return queryString;
}