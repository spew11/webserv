#include "ServerHandler.hpp"

ServerHandler::ServerHandler(Config* config): config(config)
{
	//kqueue() 생성
	kq_fd = kqueue();
	if (kq_fd == -1)
		throw std::exception();

	// Server 생성 및 changeList에 추가
	const std::vector<ServerConfig> &servConf = config->getSrvConf();
	std::cout << "test\n";
	std::map<std::pair<uint32_t, uint16_t>, int> addr_fd; //<ip, port> - fd 매핑: 중복검사
	for (std::vector<ServerConfig>::const_iterator it = servConf.begin(); it != servConf.end(); it++)
	{
		std::cout << "hello\n";
		Server *tmp = new Server(*it);
		servers.insert(std::pair<int, Server*>(tmp->getSock(), tmp));
		change_events(tmp->getSock(), EVFILT_READ, EV_ADD, 0, 0, NULL);
		// std::pair<uint32_t, uint16_t> addr(it->getIp(), it->getPort());
		// std::map<std::pair<uint32_t, uint16_t>, int>::iterator it2 = addr_fd.find(addr);
		// if (it2 != addr_fd.end()) // 중복 ip,port 존재 x
		// {
		// 	Server *tmp = new Server(*it);
		// 	servers.insert(std::pair<int, Server*>(tmp->getSock(), tmp));
		// 	change_events(tmp->getSock(), EVFILT_READ, EV_ADD, 0, 0, NULL);
		// }
		// else //중복 ip,port 존재
		// 	; // servers[it2->second]->addConf(*it); 구현필요
	}
}

ServerHandler::~ServerHandler()
{
	std::multimap<int, Server*>::iterator it = servers.begin();
	for (; it != servers.end(); it++)
		delete it->second;
	servers.clear();
	clients.clear();	
}

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
				if (it != servers.end()) //연결 요청: client 객체 생성 및  changeList에 추가
				{
					Client *cli = new Client(it->second);
					clients[cli->getSock()] = cli;
					change_events(cli->getSock(), EVFILT_READ, EV_ADD, 0, 0, NULL);
					change_events(cli->getSock(), EVFILT_WRITE, EV_ADD, 0, 0, NULL);
				}
				else //클라이언트 소켓 읽기
				{
					Client *cli = clients[curEvent->ident];
					cli->communicate();
				}
			}
			else if (curEvent->filter == EVFILT_WRITE)//클라이언트에 데이터 전송 가능
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