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
                // cout << "here: " << Utils::ltrim(lst.at(i)) << endl;
                string fieldPiece = " " + Utils::ltrim(lst.at(i));
                // cout << "fieldPiece: " << fieldPiece << endl;
                fieldTmp += fieldPiece;
                // cout << "fieldTmp: " << fieldTmp << endl;
                // cout << "headerTmp: " << headerTmp << endl;
                headers[headerTmp] = fieldTmp;
                byte += lst.at(i).length() + 2;
            }
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
