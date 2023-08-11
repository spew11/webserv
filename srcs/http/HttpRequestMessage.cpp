#include "HttpRequestMessage.hpp"

HttpRequestMessage::HttpRequestMessage(const string &requestMessage)
{
    chunkedFlag = false;
    parseRequestMessage(requestMessage);

}

void HttpRequestMessage::parseRequestMessage(const string &request)
{
    vector<string> lst = Utils::split(request, "\r\n");
    
    //start line parsing
    vector<string> tmp = Utils::split(lst.at(0), " ");
    httpMethod = tmp.at(0);
    requestTarget = tmp.at(1);
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
            tmp = Utils::split(lst.at(i), ":");
            headers.insert(make_pair(tmp.at(0), Utils::ltrim(tmp.at(1))));
            byte += lst.at(i).length() + 2;
        }
    }
    ++i;
    if (byte == request.length()) {
        body = "";
    }
    else if (this->getHeader("Transfer-Encoding") == "chunked") {
        int bodySize = stoi(lst.at(i), 0, 16);
        byte += lst.at(i).length() + 2;
        body = request.substr(byte, bodySize);
        byte += bodySize;
        string eof = Utils::trim(request.substr(byte, request.length()-byte));
        if (eof == "0") {
            chunkedFlag = false;
        }
        else {
            chunkedFlag = true;
        }
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

bool HttpRequestMessage::getChunkedFlag() const
{
    return chunkedFlag;
}