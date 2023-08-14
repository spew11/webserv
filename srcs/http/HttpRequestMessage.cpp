#include "HttpRequestMessage.hpp"
#include <fstream>
#include <stdlib.h>
HttpRequestMessage::HttpRequestMessage(const string &requestMessage)
{
    chunked = false;
    connection = true;
    errorCode = 0;
    errorCode = parseRequestMessage(requestMessage);
}

int HttpRequestMessage::parseRequestMessage(const string &request)
{
    vector<string> lst = Utils::split(request, "\r\n");

    //start line parsing
    vector<string> tmp = Utils::split(lst.at(0), " ");
    httpMethod = tmp.at(0);
    requestTarget = tmp.at(1);
    serverProtocol = tmp.at(2);
    if (serverProtocol != "HTTP/1.1") {
        return 505;
    }
    int byte = lst.at(0).length()+2; 
    
    //headers parsing 
    int i = 1;
    bool crlf = false;
    string fieldTmp;
    string headerTmp;
    for (; i < lst.size(); i++) {
        if (crlf == false && lst.at(i) == "") {
            crlf = true;
            byte += 2;
            break;
        }
        if (lst.at(i).find(":") != string::npos) {
            tmp = Utils::split(lst.at(i), ":");
            headerTmp = tmp.at(0);
            fieldTmp = Utils::ltrim(tmp.at(1));
            headers.insert(make_pair(tmp.at(0), fieldTmp));
            byte += lst.at(i).length() + 2;
        }
        else { // 헤더가 여러줄로 나눠서 오는 경우가 있음(추가 줄 앞에는 최소 하나의 스페이스 혹은 탭 문자가 와야 한다. http/1.1표준은 비추천하고있음)
            if (i > 1 and lst.at(i).find_first_of(" \t") != string::npos) {
                string fieldPiece = " " + Utils::ltrim(lst.at(i));
                fieldTmp += fieldPiece;
                headers[headerTmp] = fieldTmp;
                byte += lst.at(i).length() + 2;
            }
        }
    }
    size_t bodySize = 0;
    if (this->getHeader("Content-Length") != "") {
        bodySize = atoi((this->getHeader("Content-Length")).c_str());
    }
    else if (httpMethod == "POST" || httpMethod == "PUT" || httpMethod == "PATH") {
        if (this->getHeader("Transfer-Encoding") != "chunked") {
            return 411; // " 411 Length Required" 
        }
    }
    ++i;
    //parse body and check chunked
    if (this->getHeader("Transfer-Encoding") == "chunked") {
        if (byte == bodySize) {
            return 400; 
        }
        // chucnk가 1개만 오는 경우도 있고, 여러개가 같이 오는 경우도 있음.
        int chunkSize = 0;
        chunked = true;
        while (i < lst.size()-1) {
            if (Utils::trim(lst.at(i)) == "0") {
                chunked = false;
                break;
            }
            chunkSize = stoi(Utils::rtrim(lst.at(i)), 0, 16);
            bodySize += chunkSize;
            byte += lst.at(i).length() + 2;
            body += request.substr(byte, chunkSize);
            byte += chunkSize + 2;
            i += 2;
        }
    }
    else {
        if (byte == bodySize) {
            body = "";
        }
        else {
            body = request.substr(byte, bodySize);
        }
    }
    if (this->getHeader("Connection") == "close") {
        connection = false;
    }
    return 0;
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

int HttpRequestMessage::getErrorCode() const
{
    return errorCode;
}
