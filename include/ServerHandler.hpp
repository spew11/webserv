#ifndef SERVERHANDLER_HPP
# define SERVERHANDLER_HPP

# include <map>
# include <vector>
# include <sys/evnets.h>

class ServerHandler
{
private:
	int	kq_fd;

	std::map<int, Server*> servers;
	std::vector<Client*> clients; //map으로 변경

	ConfigParser* configparser;

    std::vector<struct kevent> changeList;
    struct kevent eventList[8];

    void change_events(std::vector<struct kevent change_list, uintptr_t ident, int16_t filter,
	uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
public:
	ServerHandler(ConfigParser* configparser);
	~ServerHandler();
	loop();
}
#endif