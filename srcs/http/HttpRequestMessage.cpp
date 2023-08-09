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

void HttpRequestMessage::parseRequestMessage(const string &requestMessage)
{
    Utils utils;
    vector<string> lst = utils.split(requestMessage, "\n\r");
    vector<string> tmp = utils.split(lst.at(0), " ");
    //start line parsing
    httpMethod = tmp.at(0);
    uri = tmp.at(1);
    serverProtocol = tmp.at(2);
    int byte = lst.at(0).length()+1; // 나중에 바디 시작 인덱스 알려면 필요
    //headers parsing
    vector<pair<string, string> > headerTmp;

    int i = 1;
    for (; i < lst.size(); i++) {
        byte += lst.at(i).length() + 1;
        bool crlf = true;
        for (int j = 0; i < lst.at(i).length(); j++) {
            string tmpS = lst.at(i);
            if (tmpS.at(j) != '\n' and tmpS.at(j) != '\r') {
                crlf = false;
                break;
            }
        }
        if (crlf == true) {
            break;
        }
        if (lst.at(i).find(":") != string::npos) {
            tmp = utils.split(lst.at(i), ":");
            headerTmp.push_back(make_pair(tmp.at(0), tmp.at(1)));
        }
        else {
            vector<pair<string, string> >::iterator lastPair = headerTmp.end()-1;
            string key = lastPair->first;
            string val = lastPair->second;
            val = val + " " + utils.ltrim(lst.at(i));
            headerTmp.pop_back();
            headerTmp.push_back(make_pair(key, val));
        }
    }
    for (vector<pair<string, string> >::iterator it = headerTmp.begin(); it != headerTmp.end(); it++) {
        headers.insert(make_pair(it->first, it->second));
    }
    byte++;
    i++;
    //body parsing
    string bodyTmp;
    if (this->getHeader("Transfer-Encoding") == "chunked") {
        if (lst.at(i) == "0") {
            chunkedFlag = false;
        }
        else {
            chunkedFlag = true;
        }
        byte += lst.at(i).length();
    }
    this->body = requestMessage.substr(byte, requestMessage.length()-byte);
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