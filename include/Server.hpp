#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <exception>
#include <unistd.h>

# include "Client.hpp"
# include "ServerConfig.hpp"

class Client;

class Server
{
private:
	int			sock;
	uint32_t	ip;
	uint16_t	port;
	std::vector<ServerConfig>	configs;

public:
	Server(const ServerConfig &config);
	~Server();

	int		getSock(void) const;
	ServerConfig::LocationMap	getConfig(std::string host) const;
};

#endif