#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <vector>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <exception>

class Server
{
private:
	int			sock;
	uint32_t	ip;
	uint16_t	port;
	std::string	domainName;

	std::vector<Client*> clients;
public:
	Server(uint32_t ip, uint16_t port, std::string domainName);
	~Server();

	int		getSock(void) const;
	bool	operator==(const std::string domainName);
	void	addClient(const Client* cli);
	void	delClient(const Client* cli);
};

#endif