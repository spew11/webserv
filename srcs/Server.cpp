#include "Server.hpp"

Server::Server(uint32_t ip, uint16_t port, std::string domainName): ip(ip), port(port), domainName(domainName)
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

Server::~Server()
{
    for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); it++)
        delete *it;
    clients.clear();
    close(sock);
}

bool	Server::operator==(const std::string domainName)
{
    if (this->domainName == domainName)
        return true;
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