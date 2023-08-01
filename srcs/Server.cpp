#include "Server.hpp"

Server::Server(uint32_t ip, uint16_t port, std::vector<std::string> serverNames, 
	std::map<std::string, LocationConfig> local): ip(ip), port(port), serverNames(serverNames), local(local)
{
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        throw std::exception();
    fcntl(sock, F_SETFL, O_NONBLOCK);
    
    struct sockaddr_in addr;
    bzero(addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(ip);
    addr.sin_addr.s_addr = htons(port);
    
    int ret = bind(sock, (struct sockaddr*)addr, sizeof(addr));
    if (ret == -1)
        throw std::exception();
    
    ret = listen(sock, 5);
    if (ret == -1)
        throw std::exception();
}

Server::Server(int sock, uint32_t ip, uint16_t port, std::vector<std::string> serverNames, 
	std::map<std::string, LocationConfig> local): sock(sock), ip(ip), port(port), serverNames(serverNames) {}

Server::~Server()
{
    for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); it++)
        delete *it;
    clients.clear();
    close(sock);
}

bool	Server::operator==(const std::string serverName)
{
    for (std::vector<std::string>::iterator it = serverNames.begin(); it != serverNames.end(); it++)
	{
		if (*it == serverName)
			return true;
	}
	return false;
}

void	Server::addClient(const Client* cli)
{
    for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); it++)
    {
        if (*it == cli)
        {
            throw std::exception();
        }
    }
    clients.push_back(cli);
}

void	Server::delClient(const Client* cli)
{
    for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); it++)
    {
        if (*it == cli)
        {
            clients.erase(it);
            delete cli;
            return
        }
    }
    throw std::exception();
}

int		Server::getSock(void) const
{
	return sock;
}

std::map<std::string, LocationConfig> Server::getLocal() const
{
	return local;
}