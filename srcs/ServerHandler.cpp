#include "ServerHandler.hpp"

ServerHandler::ServerHandler(Config* config):config(config)
{
	//kqueue() 생성
	kp_fd = kqueue();
	if (kp_fd == -1)
		throw std::exception();

	//Server 생성
	std::vector<ServerConfig> servConf = config.getServConf();
	std::map<std::string, int> ip_fd;
	for (std::vector<ServerConfig>::iterator it = servConf.begin(); it != servConf.end(); it++)
	{
		std::map<std::string, int>::iterator ipIt = ip_fd.find();
		Server *tmp;
		if (ipIt == ip_fd.end()) //겹치는 ip 없을 때
		{
			*tmp = new Server(it->getIP(), it->getPort(), it->getServerName());
			ip_fd[it->getIP()] = tmp->getSock();
		}
		else //겹치는 ip 있을 때
			*tmp = new Server(ipIt->second, it->getIP(), it->getPort(), it->getDomainName());
		servers.insert(std::pair<int, Server*>(tmp->getSock(), tmp));
		change_events(tmp->getSock(), EVFILT_READ, EV_ADD, 0, 0, NULL);
	}
}

void	ServerHandler::loop()
{
	while (true)
	{
		int new_events = kevent(kq, &changeList[0], changeList.size(), eventList, 8, NULL);
		if (new_events == -1)
			throw std::exception();

		changeList.clear();
		for (int i = 0; i < new_events; i++)
		{
			struct kevent *curEvent = &eventList[i];
			if (curEvent->flags & EV_ERROR)
			{
				if (servers.find(curEvent->ident) != servers.end())
					throw std::exception();
				else
				{
					std::map<int, Clients*>::iterator it = clients.find(curEvent->ident);
					delete *it->second;
					clients.erase(it);
				}
			}
			else if (curEvent->flags & EVFILT_READ)
			{
				std::vector<Server*>::iterator it = servers.find(curEvent->ident);
				if (it != servers.end()) //연결 요청
				{
					Client *cli = new Client();
					clients.push_back(cli);
					change_events(changeList, cli->getSock(), EVFILT_READ, EV_ADD, 0, 0, NULL);
					change_events(changeList, cli->getSock(), EVFILT_WRITE, EV_ADD, 0, 0, NULL);
				}
				else //클라이언트 데이터 받음
				{
					//클라이언트 찾기
					cli->recv();
					//최초 연결이면 서버 찾아서 연결하기
				}
			}
			else if (curEvent->flags & EVFILT_WRITE)//클라이언트에 데이터 전송 가능
			{
				//클라이언트 찾기
				cli->send();
			}
		}
	}
}

void change_events(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
	struct kevent tmp_event;
	EV_SET(&tmp_event, ident, filter, flags, fflags, data, udata);
	changeList.push_back(tmp_event);
}