#include "Client.hpp"

Client::Client(Server *server): server(server), hrb()
{
	bzero(&addr, sizeof(addr));
	sock = accept(server->getSock(), (struct sockaddr*)&addr, NULL);
	if (sock == -1)
		throw std::exception();
	fcntl(sock, F_SETFL, O_NONBLOCK);

	std::cout << "Connet: Client" << sock << std::endl;
	env.setPair("$server_addr", server->getIP());
	env.setPair("$server_port", server->getPort());
	env.setPair("$remote_addr", inet_ntoa(addr.sin_addr));
	env.setPair("$remote_port", addr.sin_port);
}

Client::~Client()
{
	close(sock);
}

void Client::send_msg()
{
	const char *tmp = send_buf.c_str();
	ssize_t len = send(sock, tmp, strlen(tmp), MSG_DONTWAIT);
	if (len == -1)
		throw std::exception();
	send_buf = "";
}

void Client::recv_msg()
{
	char tmp[1024];

	bzero(tmp, sizeof(char) * 1024);
	recv_buf = "";
	while (true)
	{
		ssize_t len = recv(sock, tmp, 1024, 0);
		if (len >= 0)
			recv_buf += std::string(tmp);
		else
			break;
	}
	std::cout << sock << ">> " << recv_buf << std::endl;
}

int Client::getSock() const
{
	return sock;
}

Server *Client::getServer() const
{
	return server;
}

std::string Client::getRecvBuf() const
{
	return recv_buf;
}

void Client::setSendBuf(std::string send_buf)
{
	this->send_buf = send_buf;
}

bool Client::isSendable() const
{
	if (send_buf.empty())
		return false;
	return true;
}

void Client::communicate()
{
	recv_msg();
	if (hrb.getNeedMoreMessageFlag() == false)
	{
		HttpRequestMessage request(send_buf);
		const ServerConfig::LocationMap lm = server->getConfig(request.getHeader("host"));
		hrb.initiate(request, env, lm);
	}
	else
	{
		hrb.addRequestMessage(recv_buf);
	}
	if (hrb.getNeedMoreMessageFlag() == true)
	{
		makeResponse();
		send_buf = hrb.getResponse().toString();
		hrb.clear();
	}
}

void Client::makeResponse()
{
	IMethodExecutor *executor;
	if (hrb.getNeedCgiFlag() == true)
	{
		ICgiScriptor *cgiScriptor = new PythonScriptor(lm.getCgiParams(env));
		executor = new CgiMethodExecutor(cgiScriptor);
	}
	else
		executor = new DefaultMethodExecutor();

	hrb.build(executor);
	delete executor;
}