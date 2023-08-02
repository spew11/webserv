#ifndef HTTP_REPONSE_MESSAGE_HPP
#define HTTP_REPONSE_MESSAGE_HPP
#include "HttpMessage.hpp"

class HttpResponseMessage : public HttpMessage
{
    private:
        int statusCode;
        string reasonPhrase;
    public:
        int getStatusCode() const;
        string getReasonPhrase() const;
        void setStatusCode(const int &statusCode);
        void setReasonPhrase(const string &reasonPhrase);
        void setBody(const string &body);
        //void toString();
};
#endif