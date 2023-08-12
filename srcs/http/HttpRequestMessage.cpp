#include "HttpRequestMessage.hpp"

HttpRequestMessage::HttpRequestMessage(const string &requestMessage)
{
    chunked = false;
    connection = true;
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
    
    //headers parsing 짤려서 오는거 고려해야됌
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

    //parse body and check flag
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
            chunked = false;
        }
        else {
            chunked = true;
        }
    }
    else {
        body = request.substr(byte, request.length()-byte);
    }

    if (this->getHeader("Connection") == "close") {
        connection = false;
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
