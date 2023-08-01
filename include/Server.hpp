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

class Client;

class Server
{
private:
	int			sock;
	uint32_t	ip;
	uint16_t	port;
	std::vector<std::string>	serverNames;
//	std::map<std::string, LocationConfig> local;

	std::vector<Client*> clients;
public:
	Server(uint32_t ip, uint16_t port, std::vector<std::string> serverNames); //, std::map<std::string, LocationConfig> local);
	Server(int sock, uint32_t ip, uint16_t port, std::vector<std::string> serverNames);//, std::map<std::string, LocationConfig> local);
	~Server();

	int		getSock(void) const;
	//std::map<std::string, LocationConfig> getLocal() const;
	
	bool	operator==(const std::string serverName);
	void	addClient(Client* cli);
	void	delClient(Client* cli);
};

#endif