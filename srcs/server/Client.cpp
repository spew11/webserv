#include "Client.hpp"

Client::Client(Server *server) : server(server)
{
	bzero(&addr, sizeof(addr));
	socklen_t cli_size = sizeof(addr);
	sock = accept(server->getSock(), (struct sockaddr *)&addr, &cli_size);
	if (sock == -1)
		throw exception();

#ifdef __APPLE__
	fcntl(sock, F_SETFL, O_NONBLOCK);
#elif __linux__
	int flags = fcntl(sock, F_GETFL, 0);
	fcntl(sock, F_SETFL, flags | O_NONBLOCK);
#endif

	cout << "Connet: Client" << sock << endl;
	cout << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port) << endl;
	webVal.insert("server_addr", server->getIP());
	webVal.insert("server_port", server->getPort());
	webVal.insert("remote_addr", inet_ntoa(addr.sin_addr));
	webVal.insert("remote_port", addr.sin_port);

	hrb = new HttpResponseBuilder(server, webVal);
}

Client::~Client()
{
	cout << "Connection Close: " << sock << endl;
	close(sock);
	delete hrb;
}

void Client::send_msg()
{
	cout << send_buf << endl;
	const char *tmp = send_buf.c_str();
	ssize_t len = send(sock, tmp, strlen(tmp), MSG_DONTWAIT);
	if (len == -1)
		throw exception();
	send_buf = "";
	recv_buf = "";
}

void Client::recv_msg()
{
	char tmp[1024];

	while (true)
	{
		bzero(tmp, sizeof(char) * 1024);
		ssize_t len = recv(sock, tmp, 1023, 0);
		if (len > 0)
			recv_buf += string(tmp);
		else
			break;
	}
	cout << sock << ">> " << recv_buf << endl;
}

int Client::getSock() const
{
	return sock;
}

Server *Client::getServer() const
{
	return server;
}

string Client::getRecvBuf() const
{
	return recv_buf;
}

void Client::setSendBuf(string send_buf)
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
	{
		return;
	}
	if (hrb->getNeedMoreMessage() == false)
	{
		hrb->initiate(recv_buf);
		// 아래 if문 하나 추가 (은지가)
		if (hrb->getEnd())
		{
			send_buf = hrb->getResponse(); // hrb에서 응답스트링 만들어주는 걸로 바꿨음.
			return;
		}
	}
	else
	{
		hrb->addRequestMessage(recv_buf);
		if (hrb->getEnd())
		{
			send_buf = hrb->getResponse();
			return;
		}
	}
	if (hrb->getNeedMoreMessage() == false)
	{
		makeResponse();
		send_buf += hrb->getResponse();
	}
}

void Client::makeResponse()
{
	IMethodExecutor *executor;
	if (hrb->getNeedCgi() == true)
	{
		LocationConfig lc = hrb->getLocationConfig();
		executor = new CgiMethodExecutor(lc.getCgiParams(webVal));
	}
	else
		executor = new DefaultMethodExecutor();
	hrb->build(*executor);
	delete executor;
}