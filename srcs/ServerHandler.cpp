#include "ServerHandler.hpp"

ServerHandler::ServerHandler(ConfigParser* configparser):configparser(configparser)
{
	kp_fd = kqueue();
	if (kp_fd == -1)
		throw std::exception();

	int cnt = configparser.getServerCnt();
	for (int i = 0; i < cnt; i++)
	{
		Server *tmp = new Server(configparser.getIp(i), configparser.getPort(i), configparser.getDomainName());
		servers[tmp->getSock()] = tmp;
		change_events(changeList, tmp->getSock(), EVFILT_READ, EV_ADD, 0, 0, NULL);
	}
}

ServerHandler::loop()
{
	while (True)
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
					//해당 클라이언트 연결 끊기
			}
			else if (curEvent->flags & EVFILT_READ)
			{
				std::vector<Server*>::iterator it = servers.find(curEvent->ident);
				if (it != servers.end())
				{
					Client *cli = new Client();
					clients.push_back(cli);
					change_events(changeList, cli->getSock(), EVFILT_READ, EV_ADD, 0, 0, NULL);
					change_events(changeList, cli->getSock(), EVFILT_WRITE, EV_ADD, 0, 0, NULL);
				}
				else
				{
					//클라이언트 찾기
					cli->recv();
				}
			}
			else if (curEvent->flags & EVFILT_WRITE)
			{
				//클라이언트 찾기
				cli->send();
			}
		}
	}
}

void change_events(std::vector<struct kevent change_list, uintptr_t ident, int16_t filter,
	uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
	struct kevent tmp_event;
	EV_SET(&tmp_event, ident, filter, flags, fflags, data, udata);
	change_list.push_back(tmp_event);
}