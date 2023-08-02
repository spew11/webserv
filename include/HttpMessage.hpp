#ifndef HTTP_MESSAGE_HPP
# define HTTP_MESSAGE_HPP
# include <iostream>
# include <string>
# include <map>

using namespace std;

class HttpMessage {
    protected:
        map<string, string> headers;
        string body;
        string serverProtocol;
    public:
        string getBody() const;
        string getServerProtocol() const;
        map<string, string> getHeaders() const;
};
#endif