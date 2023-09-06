#include "Client.hpp"

Client::Client(ServerHandler *sh, Server *server) : sh(sh), server(server), send_buf_idx(0), isBuildableFlag(false)
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

	char buf[1024];
	bzero(buf, sizeof(char) * 1024);
	size_t i = 0;
	for (; i < 1024 && i + send_buf_idx < send_buf.size(); i++)
		buf[i] = send_buf[i + send_buf_idx];

	ssize_t len = send(sock, buf, i, MSG_DONTWAIT);
	if (len != static_cast<ssize_t>(i))
		throw exception();

	if (i + send_buf_idx == send_buf.size())
	{
		cout << "***********send_buf[" << sock << "]***********" << endl;
		cout << send_buf.substr(0, send_buf.find_first_of("\r\n")) << endl;
		cout << "******************************" << endl << endl;
		send_buf = "";
		send_buf_idx = 0;
	}
	else
		send_buf_idx += i;	
}

void Client::recv_msg()
{
	char buf[1024]; 
	bzero(buf, sizeof(char) * 1024);

	ssize_t len = recv(sock, buf, 1023, 0);
	for (int i = 0; i < len; i++)
		recv_buf += buf[i];

	cout << "***************[" << sock << "]recv_buf***************" << endl;
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
				isBuildableFlag = false;
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
		isBuildableFlag = false;
	}
}