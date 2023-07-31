#include "Client.hpp"

Client::Client(int serv_sock): server(NULL)
{
	sock = accept(serv_sock, NULL, NULL);
	if (sock == -1)
		throw std::exception();
	fcntl(sock, F_SETFL, O_NONBLOCK);
}

Client::~Client()
{
	if (server)
		server->delClient(this);
	close(sock);
}

void Client::send_msg()
{
	char *tmp = send_buf.c_str();
	ssize_t len = send(sock, tmp, strlen(tmp), MSG_DONTWAIT);
	if (len == -1)
		throw std::exception();
	send_buf = "";
}

void Client::recv_msg()
{
	char tmp[1024];

	recv_buf = "";
	while (true)
	{
		ssize_t len = recv(sock, tmp, 1024, 0);
		if (len == 1023)
			recv_buf += std::string(tmp);
		else if (len >= 0)
			break;
		else
			throw std::exception();
	}
	if (!server)
		// find_server
	ResponseBuilder rb(recv_buf, server->getLocationMap());
	//???
	//send_buf = rb->getResponse()->toString();
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
	if (send_buf == "")
		return false;
	return true;
}