#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <exception>
#include <sys/socket.h>

#include "ServerHandler.hpp"
 #include "Server.hpp"
 #include "HttpResponseBuilder.hpp"
 #include "DefaultMethodExecutor.hpp"
 #include "CgiMethodExecutor.hpp"
 #include "WebservValues.hpp"
 #include "HttpRequestBuilder.hpp"

using namespace std;

class ServerHandler;
class Server;
class HttpResponseBuilder;

class Client
{
private:
	ServerHandler *sh;
	Server *server;
	HttpResponseBuilder *hrb;
	HttpRequestBuilder *httpRequestBuilder;
	WebservValues webVal;

	int sock;
	struct sockaddr_in addr;

	string send_buf;
	string recv_buf;

	bool isBuildableFlag;

public:
	Client(ServerHandler *sh, Server *server);
	~Client();

	void send_msg();
	void recv_msg();
	void communicate();

	int getSock() const;
	Server *getServer() const;
	string getRecvBuf() const;
	bool getConnection() const;

	void setSendBuf(string send_buf);
	bool isSendable() const;
	bool isBuildable() const;

	void makeResponse();
};

#endif