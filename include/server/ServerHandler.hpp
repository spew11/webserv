#ifndef SERVERHANDLER_HPP
# define SERVERHANDLER_HPP

# include <vector>
# include <map>

# ifdef __APPLE__
#  include <sys/event.h>
#  define NEVENTS 8
# elif __linux__
#  include <sys/poll.h>
# endif

#include "Server.hpp"
#include "Client.hpp"
#include "Config.hpp"

using namespace std;

class Server;
class Client;

class ServerHandler
{
private:
#ifdef __APPLE__
	// kqueue(), kevent() 관련 변수
	int kq_fd;
	vector<struct kevent> changeList; // kqueue 변동 이벤트 (추가 삭제 등)
	struct kevent eventList[8];			   // kevent()에서 발생한 이벤트 리턴

	void handleServerEvent(struct kevent &curEvent, Server *server);
	void handleClientEvent(struct kevent &curEvent, Client *client);
	void handleBuildEvent(struct kevent &curEvent);

#elif __linux__
	vector<struct pollfd> fds;
#endif

	map<int, Server *> servers; // sockfd, server
	map<int, Client *> clients; // sockfd, client

	Config *config;


public:
	ServerHandler(Config *config);
	~ServerHandler();
	void loop();
	void change_events(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
};

#endif