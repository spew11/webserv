#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <exception>
# include <sys/socket.h>

# include "Server.hpp"
# include "HttpResponseBuilder.hpp"
# include "DefaultMethodExecutor.hpp"
# include "CgiMethodExecutor.hpp"
# include "WebservValues.hpp"

class Server;

class Client
{
private:
	Server *server;
	HttpResponseBuilder *hrb;
	WebservValues webVal;

	int sock;
	struct sockaddr_in addr;

	std::string send_buf;
	std::string recv_buf;

	ServerConfig::LocationMap lm;
public:
	Client(Server *server);
	~Client();

	void send_msg();
	void recv_msg();
	void communicate();

	int getSock() const;
	Server *getServer() const;
	std::string getRecvBuf() const;
	void setSendBuf(std::string send_buf);
	bool isSendable() const;

private:
	void makeResponse();
};

#endif