#include "Client.hpp"

Client::Client(ServerHandler *sh, Server *server) : sh(sh), server(server), send_buf_idx(0), isBuildableFlag(false)
{
	bzero(&addr, sizeof(addr));
	socklen_t cli_size = sizeof(addr);
	sock = accept(server->getSock(), (struct sockaddr *)&addr, &cli_size);
	if (sock == -1)
		throw exception();

#ifdef __APPLE__
	fcntl(sock, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
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
	if (len < 0)
		return;

	if (static_cast<size_t>(len) + send_buf_idx == send_buf.size())
	{
		cout << "***********send_buf[" << sock << "]***********" << endl;
		cout << send_buf.substr(0, send_buf.find_first_of("\r\n")) << endl;
		cout << "******************************" << endl << endl;
		send_buf = "";
		send_buf_idx = 0;
	}
	else
		send_buf_idx += static_cast<size_t>(len);
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
	else if (ret == -1)
	{
		hrb->initiate(NULL);
		send_buf = hrb->getResponse();
		return ;
	}
	else if (ret == 0)
	{
		if (hrb->getNeedMoreMessage() == false)
		{
			cout << "정상적인 요청" << endl;
			hrb->initiate(httpRequestBuilder->createRequestMessage());
		}
		else
		{
			hrb->addRequestMessage(httpRequestBuilder->createRequestMessage());
		}

		if (hrb->getNeedMoreMessage() == true)
		{
			return ;
		}
	}
	
	LocationConfig lc = hrb->getLocationConfig();
	if (hrb->getEnd())
	{
		// 구현되지 않은 요청을 받은 경우에는 requestMessage 객체에 firstLine밖에 안들어있어서 애초에 lc를 확인할 수 없음
		if (lc.isErrCode(hrb->getStatusCode()) && hrb->getRequestMessage().getHeaders().size())
		{
			hrb->changeRequestMessage(hrb->getStatusCode());
		}
		else
		{
			hrb->createResponseMessage();
			send_buf = hrb->getResponse();
			return ;
		}
	}
	
	IMethodExecutor *executor;
	if (hrb->getNeedCgi() == true)
	{
		lc = hrb->getLocationConfig();
		executor = new CgiMethodExecutor(sh, this, lc.getCgiParams(webVal));
	}
	else
		executor = new DefaultMethodExecutor(sh, this);
	hrb->setMethodExecutor(executor);
	isBuildableFlag = true;
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