#ifndef SERVERHANDLER_HPP
#define SERVERHANDLER_HPP

#include <vector>
#include <map>
#include <sys/event.h>
#include "Server.hpp"

# define NEVENTS 8
class ServerHandler
{
private:
	// kqueue(), kevent() 관련 변수
	int kq_fd;
	std::vector<struct kevent> changeList; // kqueue 변동 이벤트 (추가 삭제 등)
	struct kevent eventList[8];			   // kevent()에서 발생한 이벤트 리턴

	std::multimap<int, Server *> servers; // sockfd, server
	std::map<int, Clients *> clients;	  // sockfd, client

	Config *config;

	void change_events(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);

public:
	ServerHandler(Config *config);
	~ServerHandler();
	void loop();
};

#endif