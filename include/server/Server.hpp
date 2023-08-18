#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <exception>
#include <unistd.h>

#include "Client.hpp"
#include "ServerConfig.hpp"

using namespace std;

class Client;

class Server
{
private:
	int sock;
	struct sockaddr_in addr;
	vector<ServerConfig> configs;

public:
	Server(const ServerConfig &config);
	~Server();

	bool isSame(const uint32_t &ip, const uint16_t &port);
	void addConfig(const ServerConfig &config);
	int getSock(void) const;
	ServerConfig::LocationMap getConfig(string host) const;
	string getIP(void) const;
	uint16_t getPort(void) const;
};

#endif