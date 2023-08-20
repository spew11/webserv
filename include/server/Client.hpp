#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <exception>
#include <sys/socket.h>

 #include "Server.hpp"
 #include "HttpResponseBuilder.hpp"
 #include "DefaultMethodExecutor.hpp"
 #include "CgiMethodExecutor.hpp"
 #include "WebservValues.hpp"
 #include "HttpRequestBuilder.hpp"

using namespace std;

class HttpResponseBuilder;
class Server;

class Client
{
private:
	Server *server;
	HttpResponseBuilder *hrb;
	HttpRequestBuilder *httpRequestBuilder;
	WebservValues webVal;

	int sock;
	struct sockaddr_in addr;

	string send_buf;
	string recv_buf;

public:
	Client(Server *server);
	~Client();

	void send_msg();
	void recv_msg();
	void communicate();

	int getSock() const;
	Server *getServer() const;
	string getRecvBuf() const;
	void setSendBuf(string send_buf);
	bool isSendable() const;

private:
	void makeResponse();
};

#endif