#include "HttpRequestMessage.hpp"

HttpRequestMessage::HttpRequestMessage(const string &requestMessage)
{
    chunkedFlag = false;
    requestMessageParser(requestMessage);
}

void HttpRequestMessage::requestMessageParser(const string &requestMessage)
{
    Utils utils;
    vector<string> list = utils.split(requestMessage, "\n\r");
    vector<string> tmp = utils.split(list.at(0), " ");
    //start line parsing
    method = tmp.at(0);
    uri = tmp.at(1);
    serverProtocol = tmp.at(2);
    int byte = list.at(0).length()+1; // 나중에 바디 시작 인덱스 알려면 필요
    //headers parsing
    vector<pair<string, string> > headerTmp;

    int i = 1;
    for (; i < list.size(); i++) {
        byte += list.at(i).length() + 1;
        bool crlf = true;
        for (int j = 0; i < list.at(i).length(); j++) {
            string tmpS = list.at(i);
            if (tmpS.at(j) != '\n' and tmpS.at(j) != '\r') {
                crlf = false;
                break;
            }
        }
        if (crlf == true) {
            break;
        }
        if (list.at(i).find(":") != string::npos) {
            tmp = utils.split(list.at(i), ":");
            headerTmp.push_back(make_pair(tmp.at(0), tmp.at(1)));
        }
        else {
            vector<pair<string, string> >::iterator lastPair = headerTmp.end()-1;
            string key = lastPair->first;
            string val = lastPair->second;
            val = val + " " + utils.ltrim(list.at(i));
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
    if (headers.find("Transfer-Encoding") != headers.end()) {
        chunkedFlag = static_cast<bool>(stoi(list.at(i), nullptr, 16));
        byte += list.at(i).length();
    }
    this->body = requestMessage.substr(byte, requestMessage.length()-byte);
}

string HttpRequestMessage::getMethod() const
{
    return method;
}

string HttpRequestMessage::getUri() const
{
    return uri;
}

int HttpRequestMessage::getChunkedFlag() const
{
    return chunkedFlag;
}