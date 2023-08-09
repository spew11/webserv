#include "Client.hpp"

Client::Client(Server *server): server(server), hrb()
{
	bzero(&addr, sizeof(addr));
	socklen_t cli_size = sizeof(addr);
	sock = accept(server->getSock(), (struct sockaddr*)&addr, &cli_size);
	if (sock == -1)
		throw std::exception();
	fcntl(sock, F_SETFL, O_NONBLOCK);

	std::cout << "Connet: Client" << sock << std::endl;
	std::cout << inet_ntoa(addr.sin_addr) << ":" << addr.sin_port << std::endl;
	webVal.insert("server_addr", server->getIP());
	webVal.insert("server_port", server->getPort());
	webVal.insert("remote_addr", inet_ntoa(addr.sin_addr));
	webVal.insert("remote_port", addr.sin_port);
	
	// hrb = new HttpResponseBuilder(webVal, server);
}

Client::~Client()
{
	close(sock);
}

void Client::send_msg()
{
	std::cout << send_buf << std::endl;
	const char *tmp = send_buf.c_str();
	ssize_t len = send(sock, tmp, strlen(tmp), MSG_DONTWAIT);
	if (len == -1)
		throw std::exception();
	send_buf = "";
	recv_buf = "";
}

void Client::recv_msg()
{
	char tmp[1024];

	bzero(tmp, sizeof(char) * 1024);
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
	if (recv_buf.find("\r\n\r\n") == string::npos)
		return;
	if (hrb.getNeedMoreMessageFlag() == false)
	{
		///
		HttpRequestMessage *request;
		request = new HttpRequestMessage(recv_buf);
		lm = server->getConfig(request->getHeader("host"));
		hrb.initiate(*request, webVal, lm);
		///
		// hrb.initiate(recv_buf);
	}
	else
	{
		hrb.addRequestMessage(recv_buf);
	}

	if (hrb.getNeedMoreMessageFlag() == false)
	{
		makeResponse();
		send_buf = hrb.getResponseMessage().toString();
		hrb.clear();
	}
}

void Client::makeResponse()
{
	IMethodExecutor *executor;
	if (hrb.getNeedCgiFlag() == true)
	{
		LocationConfig lc = lm.getLocConf("cgi-bin/test.py"); //수정 필요!!!!!!
		char **tmp = lc.getCgiParams(webVal);
		executor = new CgiMethodExecutor(tmp);
		// excutor = lm.getLocConf("")
		// executor = new CgiMethodExecutor(hrb.getEnv()); ///?
	}
	else
		executor = new DefaultMethodExecutor();

	hrb.build(*executor);
	delete executor;
}