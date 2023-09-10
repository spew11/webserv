#include "ServerHandler.hpp"

#ifdef __APPLE__
ServerHandler::ServerHandler(Config *config) : config(config)
{
	// kqueue() 생성
	kq_fd = kqueue();
	if (kq_fd == -1)
		throw exception();
	// Server 생성 및 changeList에 추가
	const vector<ServerConfig> &servConf = this->config->getSrvConf();
	for (vector<ServerConfig>::const_iterator it = servConf.begin(); it != servConf.end(); it++)
	{
		map<int, Server *>::iterator it2 = servers.begin();
		for (; it2 != servers.end(); it2++)
			if (it2->second->isSame(it->getIp(), it->getPort()))
				break;

		if (it2 == servers.end()) // 중복 ip,port 존재 x
		{
			Server *tmp = new Server(*it);
			servers.insert(pair<int, Server *>(tmp->getSock(), tmp));
			change_events(tmp->getSock(), EVFILT_READ, EV_ADD, 0, 0, NULL);
		}
		else // 중복 ip,port 존재
			servers[it2->first]->addConfig(*it);
	}
}

#elif __linux__
ServerHandler::ServerHandler(Config *config) : config(config)
{
	const vector<ServerConfig> &servConf = config->getSrvConf();
	for (vector<ServerConfig>::const_iterator it = servConf.begin(); it != servConf.end(); it++)
	{
		map<int, Server *>::iterator it2 = servers.begin();
		for (; it2 != servers.end(); it2++)
			if (it2->second->isSame(it->getIp(), it->getPort()))
				break;
		if (it2 == servers.end()) // 중복 ip,port 존재 x
		{
			Server *tmp = new Server(*it);
			servers.insert(pair<int, Server *>(tmp->getSock(), tmp));
			struct pollfd pfd = {tmp->getSock(), POLLIN, 0};
			fds.push_back(pfd);
		}
		else // 중복 ip,port 존재
			servers[it2->first]->addConfig(*it);
	}
}
#endif

ServerHandler::~ServerHandler()
{
	multimap<int, Server *>::iterator it = servers.begin();
	for (; it != servers.end(); it++)
		delete it->second;
	servers.clear();
	clients.clear();
}

#ifdef __APPLE__
void ServerHandler::loop()
{
	while (true)
	{
		struct timespec timeout = {5, 0};
		int new_events = kevent(kq_fd, &changeList[0], changeList.size(), eventList, NEVENTS, &timeout);
		if (new_events == -1)
			throw exception();
		else if (new_events == 0)
		{
			cout << "timeout" << endl;
			continue;
		}

		changeList.clear();
		for (int i = 0; i < new_events; i++)
		{
			struct kevent curEvent = eventList[i];

			cout << "EVENT OCCUR = " << curEvent.ident << endl;
			if (curEvent.udata != NULL)
			{
				handleBuildEvent(curEvent);
				continue;
			}

			map<int, Server*>::iterator servIt = servers.find(curEvent.ident);
			if (servIt != servers.end())
			{
				handleServerEvent(curEvent, servIt->second);
				continue;
			}

			map<int, Client*>::iterator cliIt = clients.find(curEvent.ident);
			if (cliIt != clients.end())
			{
				handleClientEvent(curEvent, cliIt->second);
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

void ServerHandler::handleServerEvent(struct kevent &curEvent, Server *server)
{
	if (curEvent.filter == EVFILT_READ && curEvent.data > 0)
	{
		try
		{
			Client *cli = new Client(this, server);
			clients[cli->getSock()] = cli;
			change_events(cli->getSock(), EVFILT_READ, EV_ADD, 0, 0, NULL);
			change_events(cli->getSock(), EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
			change_events(cli->getSock(), EVFILT_TIMER, EV_ADD, NOTE_SECONDS, 120, NULL);
		}
		catch(const exception& e)
		{
			cerr << "Fail to Create Client...: " << endl;
		}
	}
}

void ServerHandler::handleClientEvent(struct kevent &curEvent, Client *client)
{
	if (curEvent.filter == EVFILT_TIMER || curEvent.flags & (EV_EOF | EV_ERROR))
	{
		clients.erase(client->getSock());
		delete client;
	}
	else if (curEvent.filter == EVFILT_READ)
	{
		client->communicate();
		if (client->isSendable())
		{
			change_events(client->getSock(), EVFILT_READ, EV_DISABLE, 0, 0, NULL);
			change_events(client->getSock(), EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
		}
		else if (client->isBuildable())
		{
			client->makeResponse(-1);
			change_events(client->getSock(), EVFILT_READ, EV_DISABLE, 0, 0, NULL);
			if (client->isSendable())
				change_events(client->getSock(), EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
		}
	}
	else if (curEvent.filter == EVFILT_WRITE)
	{
		if (client->isSendable())
		{
			client->send_msg();
			if (client->getConnection() == false)
			{
				map<int, Client*>::iterator it = clients.find(client->getSock());
				delete it->second;
				clients.erase(it);
				return ;
			}
			// client가 소켓에 쓰기를 완료한 경우 이벤트 처리
			if (!client->isSendable())
			{
				change_events(client->getSock(), EVFILT_READ, EV_ENABLE, 0, 0, NULL);
				change_events(client->getSock(), EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
			}
		}

	}
}

void ServerHandler::handleBuildEvent(struct kevent &curEvent)
{
	Client *client = reinterpret_cast<Client*>(curEvent.udata);
	int exitCode = (curEvent.filter == EVFILT_PROC)? curEvent.data : -1;
	
	client->makeResponse(exitCode);
	if (client->isSendable())
		change_events(client->getSock(), EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
}

#elif __linux__
void ServerHandler::loop()
{
	while (true)
	{
		int activity = poll(fds.data(), fds.size(), -1);
		if (activity < 0)
			throw exception();

		for (size_t i = 0; i < fds.size(); i++)
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
					map<int, Client *>::iterator it = clients.find(fds[i].fd);
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