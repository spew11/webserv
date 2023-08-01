#include "ServerHandler.hpp"

ServerHandler::ServerHandler(Config* config):config(config)
{
	//kqueue() 생성
	kp_fd = kqueue();
	if (kp_fd == -1)
		throw std::exception();

	//Server 생성 및 changeList에 추가
	const std::vector<ServerConfig> &servConf = config.getServConf();
	std::map<std::string, int> ip_fd; //ip별 sd 저장
	for (std::vector<ServerConfig>::iterator it = servConf.begin(); it != servConf.end(); it++)
	{
		Server *tmp;
		std::map<std::string, int>::iterator itIp = ip_fd.find(it->getIP());
		if (itIp == ip_fd.end()) //겹치는 ip 없을 때
		{
			*tmp = new Server(it->getIP(), it->getPort(), it->getServerNames());
			ip_fd[it->getIP()] = tmp->getSock();
			change_events(tmp->getSock(), EVFILT_READ, EV_ADD, 0, 0, NULL);
		}
		else //겹치는 ip 있을 때
			*tmp = new Server(itIp->second, it->getIP(), it->getPort(), it->getServerNames());
		servers.insert(std::pair<int, Server*>(tmp->getSock(), tmp));
	}
}

void	ServerHandler::loop()
{
	while (true)
	{
		int new_events = kevent(kq, &changeList[0], changeList.size(), eventList, NEVENTS, NULL);
		if (new_events == -1)
			throw std::exception();

		changeList.clear();
		for (int i = 0; i < new_events; i++)
		{
			struct kevent *curEvent = &eventList[i];
			if (curEvent->flags & EV_ERROR) //에러 발생한 경우
			{
				if (servers.find(curEvent->ident) != servers.end())
					throw std::exception();
				else //클라이언트 소켓에 에러 -> 연결 끊고 삭제
				{
					std::map<int, Clients*>::iterator it = clients.find(curEvent->ident);
					delete it->second;
					clients.erase(it);
				}
			}
			else if (curEvent->flags & EVFILT_READ) //읽기 이벤트 발생
			{
				std::vector<Server*>::iterator it = servers.find(curEvent->ident);
				if (it != servers.end()) //연결 요청: client 객체 생성 및  changeList에 추가
				{
					Client *cli = new Client(it->first);
					clients[cli->getSock()] = cli;
					change_events(cli->getSock(), EVFILT_READ, EV_ADD, 0, 0, NULL);
					change_events(cli->getSock(), EVFILT_WRITE, EV_ADD, 0, 0, NULL);
				}
				else //클라이언트 소켓 읽기
				{
					Client *cli = clients[curEvent->ident];
					cli->recv_msg();
				}
			}
			else if (curEvent->flags & EVFILT_WRITE)//클라이언트에 데이터 전송 가능
			{
				Client *cli = clients[curEvent->ident];
				if (cli->isSendable())
					cli->send_msg();
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