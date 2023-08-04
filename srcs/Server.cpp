#include "Server.hpp"

Server::Server(const ServerConfig &config) : ip(config.getIp()), port(config.getPort())
{
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		throw std::exception();
	fcntl(sock, F_SETFL, O_NONBLOCK);

	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(ip);
	addr.sin_port = htons(port);

	int ret = ::bind(sock, (struct sockaddr *)&addr, sizeof(addr));
	if (ret == -1)
		throw std::exception();

	ret = listen(sock, 5);
	if (ret == -1)
		throw std::exception();

	configs.push_back(config);
}

Server::~Server()
{
	close(sock);
}

int Server::getSock(void) const
{
	return sock;
}

ServerConfig Server::getCofig(std::string host) const
{
	std::vector<ServerConfig>::const_iterator it = configs.begin();
	for (; it != configs.end(); it++)
		if (*it == host)
			return *it;
	return *(configs.begin());
}