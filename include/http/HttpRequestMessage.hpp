#ifndef HTTP_REQUEST_MESSAGE_HPP
#define HTTP_REQUEST_MESSAGE_HPP
#include "HttpMessage.hpp"
#include "Utils.hpp"

class HttpRequestMessage : public HttpMessage
{
private:
  string httpMethod;
  string requestTarget;
  bool chunked;
  bool connection;
  int errorCode;
public:
  void setFlag();
  HttpRequestMessage(const string httpMethod, const string requestTarget, \
  string serverProtocol, map<string, string> headers, string body);
  // HttpRequestMessage(const string &requestMessage);
  string getHttpMethod() const;
  string getRequestTarget() const;
  bool getChunked() const;
  bool getConnection() const;
  int getErrorCode() const;
};

#endif