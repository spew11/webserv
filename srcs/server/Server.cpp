#include "Server.hpp"

Server::Server(const ServerConfig &config)
{
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		throw std::exception();
	fcntl(sock, F_SETFL, O_NONBLOCK);

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(config.getIp());
	addr.sin_port = htons(config.getPort());

	int ret = ::bind(sock, (struct sockaddr *)&addr, sizeof(addr));
	if (ret == -1)
		throw std::exception();

	ret = listen(sock, 5);
	if (ret == -1)
		throw std::exception();

	configs.push_back(config);
	std::cout << "create Server:" << inet_ntoa(addr.sin_addr) << ":" << addr.sin_port << std::endl;
}

bool Server::isSame(const uint32_t &ip, const uint16_t &port)
{
	if (addr.sin_addr.s_addr == ip && addr.sin_port == port)
		return true;
	return false;
}

Server::~Server()
{
	close(sock);
}

int Server::getSock(void) const
{
	return sock;
}

ServerConfig::LocationMap Server::getConfig(std::string host) const
{
	std::vector<ServerConfig>::const_iterator it = configs.begin();
	for (; it != configs.end(); it++)
		if (*it == host)
			return it->getLocationMap();
	return (configs.begin())->getLocationMap();
}

string	Server::getIP(void) const
{
	return inet_ntoa(addr.sin_addr);
}

uint16_t	Server::getPort(void) const
{
	return addr.sin_port;
}

void	Server::addConfig(const ServerConfig &config)

{
	// ServerConfig의 operator== 구현되어야 함.
	// if (find(configs.begin(), configs.end(), config) == configs.end())
	// 	throw std::exception();
	configs.push_back(config);
}