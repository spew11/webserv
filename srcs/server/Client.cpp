#include "Client.hpp"

Client::Client(ServerHandler *sh, Server *server) : sh(sh), server(server), isBuildableFlag(false)
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

	cout << "Connect: Client" << sock << endl;
	cout << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port) << endl;
	webVal.insert("server_addr", server->getIP());
	webVal.insert("server_port", server->getPort());
	webVal.insert("remote_addr", inet_ntoa(addr.sin_addr));
	webVal.insert("remote_port", addr.sin_port);

	hrb = new HttpResponseBuilder(server, webVal);
	httpRequestBuilder = new HttpRequestBuilder();
}

Client::~Client()
{
	cout << "Connection Close: " << sock << endl;
	close(sock);
	delete hrb;
	delete httpRequestBuilder;
}

void Client::send_msg()
{
	cout << "***********send_buf[" << sock << "]***********" << endl;
	cout << send_buf << endl;
	cout << "******************************" << endl << endl;
	const char *tmp = send_buf.c_str();
	ssize_t len = send(sock, tmp, strlen(tmp), MSG_DONTWAIT);
	if (len == -1)
		throw exception();
	send_buf = "";
	isBuildableFlag = false;
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
	cout << "***************[" << sock << "]recv_buf***************" << endl;
	// cout << sock << ">> " << recv_buf << endl;
	cout << recv_buf << endl;
	cout << "******************************" << endl << endl;
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

bool Client::getConnection() const
{
	return hrb->getConnection();
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

bool Client::isBuildable() const
{
	return isBuildableFlag;
}

void Client::communicate()
{
	recv_msg();	
	int ret = httpRequestBuilder->isHttp(recv_buf);
	if (ret == 1)
	{
		return ;
	}
	if (ret == -1)
	{
		hrb->createInvalidResponseMessage();
		send_buf = hrb->getResponse();
		return ;
	}
	if (ret == 0) {
		if (hrb->getNeedMoreMessage() == false)
		{
			// httpRequestBuilder->print();
			hrb->initiate(httpRequestBuilder->createRequestMessage());

			if (hrb->getEnd())
			{
				send_buf = hrb->getResponse();
				return;
			}
		}
		else
		{
			hrb->addRequestMessage(httpRequestBuilder->createRequestMessage());
			if (hrb->getEnd())
			{
				send_buf = hrb->getResponse();
				return ;
			}
		}
		if (hrb->getNeedMoreMessage() == false)
		{
			IMethodExecutor *executor;
			if (hrb->getNeedCgi() == true)
			{
				LocationConfig lc = hrb->getLocationConfig();
				executor = new CgiMethodExecutor(sh, this, lc.getCgiParams(webVal));
			}
			else
				executor = new DefaultMethodExecutor(sh, this);
			hrb->setMethodExecutor(executor);
			isBuildableFlag = true;
		}
	}
}

void Client::makeResponse(const int &exitCode)
{
	hrb->build(exitCode);
	if (hrb->getEnd())
	{
		this->send_buf = hrb->getResponse();
		// hrb->deleteMethodExecutor();
	}
}