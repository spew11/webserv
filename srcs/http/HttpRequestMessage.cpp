#include "HttpRequestMessage.hpp"
#include <fstream>
#include <stdlib.h>
HttpRequestMessage::HttpRequestMessage(const string &requestMessage)
{
    cout << "start" << endl;
    chunked = false;
    connection = true;
    errorCode = 0;
    errorCode = parseRequestMessage(requestMessage);
}

int HttpRequestMessage::parseRequestMessage(const string &request)
{
    // if (request.find("\r\n\r\n") == string::npos) {
    //     return 400;
    // } -> 이걸 하면 그냥 처음 get 테스트 부터 터짐
    vector<string> lst = Utils::split(request, "\r\n");

    //start line parsing
    vector<string> tmp = Utils::split(lst.at(0), " ");
    httpMethod = tmp.at(0);
    requestTarget = tmp.at(1);
    serverProtocol = tmp.at(2);
    ofstream outfile("outfile");
    outfile << serverProtocol << "\n";
    if (serverProtocol != "HTTP/1.1") {
        if (outfile.is_open()) {
            outfile << "inVALID!!\n";
        }
        return 505;
    }
    int byte = lst.at(0).length()+2; // 나중에 바디 시작 인덱스 알려면 필요
    
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
    for (auto it = this->headers.begin(); it != this->headers.end(); it++) {
        cout << it->first << " " << it->second << endl;
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
    cout << this->getHeader("Transfer-Encoding") << endl;
    ++i;
    //parse body and check flag
    if (byte == bodySize) {
        body = "";
    }
    else if (this->getHeader("Transfer-Encoding") == "chunked") {
        // chucnk가 1개만 오는 경우도 있고, 여러개가 같이 오는 경우도 있음.
        int chunkSize = 0;
        chunked = true;
        // cout << "lis size: " << lst.size() << endl;
        while (i < lst.size()-1) {
            // cout << "lst.(" << i << ") : " << lst.at(i) << endl;  
            if (Utils::trim(lst.at(i)) == "0") {
                chunked = false;
                break;
            }
            chunkSize = stoi(Utils::rtrim(lst.at(i)), 0, 16);
            bodySize += chunkSize;
            byte += lst.at(i).length() + 2;
            body += request.substr(byte, chunkSize);
            // cout << "body: " << body << endl;
            byte += chunkSize + 2;
            i += 2;
        }
        // cout << "Body size: " << bodySize << endl;
        // cout << "body : " << body << endl;
        // cout << "chunked:  " << chunked << endl;
    }
    else {
        body = request.substr(byte, bodySize);
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
