#include "ServerHandler.hpp"

#ifdef __APPLE__
ServerHandler::ServerHandler(Config* config): config(config)
{
	//kqueue() 생성
	kq_fd = kqueue();
	if (kq_fd == -1)
		throw std::exception();

	// Server 생성 및 changeList에 추가
	const std::vector<ServerConfig> &servConf = config->getSrvConf();
	for (std::vector<ServerConfig>::const_iterator it = servConf.begin(); it != servConf.end(); it++)
	{
		std::map<int, Server*>::iterator it2 = servers.begin();
		for (; it2 != servers.end(); it2++)
			if (it2->second->isSame(it->getIp(), it->getPort()))
				break;
	
		if (it2 == servers.end()) // 중복 ip,port 존재 x
		{
			Server *tmp = new Server(*it);
			servers.insert(std::pair<int, Server*>(tmp->getSock(), tmp));
			change_events(tmp->getSock(), EVFILT_READ, EV_ADD, 0, 0, NULL);
		}
		else //중복 ip,port 존재
			servers[it2->first]->addConfig(*it);
	}
}

#elif __linux__
ServerHandler::ServerHandler(Config* config): config(config)
{
	const std::vector<ServerConfig> &servConf = config->getSrvConf();
	for (std::vector<ServerConfig>::const_iterator it = servConf.begin(); it != servConf.end(); it++)
	{
		std::map<int, Server*>::iterator it2 = servers.begin();
		for (; it2 != servers.end(); it2++)
			if (it2->second->isSame(it->getIp(), it->getPort()))
				break;
		if (it2 == servers.end()) // 중복 ip,port 존재 x
		{
			Server *tmp = new Server(*it);
			servers.insert(std::pair<int, Server*>(tmp->getSock(), tmp));
			struct pollfd pfd = {tmp->getSock(), POLLIN, 0};
			fds.push_back(pfd);
		}
		else //중복 ip,port 존재
			servers[it2->first]->addConfig(*it);
	}
}

#endif

ServerHandler::~ServerHandler()
{
	std::multimap<int, Server*>::iterator it = servers.begin();
	for (; it != servers.end(); it++)
		delete it->second;
	servers.clear();
	clients.clear();	
}

#ifdef __APPLE__
void	ServerHandler::loop()
{
	while (true)
	{
		int new_events = kevent(kq_fd, &changeList[0], changeList.size(), eventList, NEVENTS, NULL);
		if (new_events == -1)
			throw std::exception();

		changeList.clear();
		for (int i = 0; i < new_events; i++)
		{
			struct kevent *curEvent = &eventList[i];
			if (curEvent->flags & EV_EOF || curEvent->flags & EV_ERROR) //에러 발생한 경우
			{
				if (servers.find(curEvent->ident) != servers.end())
					throw std::exception();
				else //클라이언트 소켓에 에러 -> 연결 끊고 삭제
				{
					std::map<int, Client*>::iterator it = clients.find(curEvent->ident);
					delete it->second;
					clients.erase(it);
				}
			}
			else if (curEvent->filter == EVFILT_READ) //읽기 이벤트 발생
			{
				std::map<int, Server*>::iterator it = servers.find(curEvent->ident);
				std::map<int, Client*>::iterator it2 = clients.find(curEvent->ident);
				if (it != servers.end()) //연결 요청: client 객체 생성 및  changeList에 추가
				{
					Client *cli = new Client(it->second);
					clients[cli->getSock()] = cli;
					change_events(cli->getSock(), EVFILT_READ, EV_ADD, 0, 0, NULL);
					change_events(cli->getSock(), EVFILT_WRITE, EV_ADD, 0, 0, NULL);
				}
				else if (it2 != clients.end()) //클라이언트 소켓 읽기
				{
					Client *cli = it2->second;
					cli->communicate();
				}
			}
			else if (curEvent->filter == EVFILT_WRITE)//클라이언트에 데이터 전송 가능
			{
				std::map<int, Client*>::iterator it2 = clients.find(curEvent->ident);
				if (it2 != clients.end() && it2->second->isSendable())
					it2->second->send_msg();
			}
		}
	}
}

void ServerHandler::change_events(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
	struct kevent tmp_event;
	EV_SET(&tmp_event, ident, filter, flags, fflags, data, udata);
	changeList.push_back(tmp_event);
}

#elif __linux__
void	ServerHandler::loop()
{
	while (true)
	{
		int activity = poll(fds.data(), fds.size(), -1);
		if (activity < 0)
			throw std::exception();
		
		for (int i = 0; i < fds.size(); i++)
		{
			if (fds[i].revents & (POLL_ERR | POLL_HUP))
			{
				close(fds[i].fd);
				fds[i].fd = -1;
			}
			if (fds[i].revents & POLLIN)
			{
				if (i < servers.size())
				{
					Client *cli = new Client(servers[fds[i].fd]);
					clients[cli->getSock()] = cli;
					struct pollfd pfd = {cli->getSock(), POLLIN, 0};
					fds.push_back(pfd);
				}
				else
				{
					std::map<int, Client*>::iterator it = clients.find(fds[i].fd);
					if (it != clients.end())
					{
						Client *cli = it->second;

						cli->communicate();
						if (cli->isSendable())
							cli->send_msg();
					}
				}
			}
		}
	}
}
#endif