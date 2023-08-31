#include "DefaultMethodExecutor.hpp"

DefaultMethodExecutor::DefaultMethodExecutor(ServerHandler *sh, Client *client): sh(sh), client(client), step(STEP_OPEN_FILE), fd(-1) {}

int DefaultMethodExecutor::getMethod(const string &resourcePath, string &response)
{
	if (step == STEP_OPEN_FILE)
	{
		fd = open(resourcePath.c_str(), O_RDONLY | O_NONBLOCK);
		if (fd == -1)
			return 500;
		step = STEP_IO_OPER;
		sh->change_events(fd, EVFILT_READ, EV_ADD, 0, 0, reinterpret_cast<void*>(client));
	}
	else if (step == STEP_IO_OPER)
	{
		char buf[1024];
		bzero(buf, 1024);
		int len = read(fd, buf, 1023);
		if (len < 0)
			return 500;
		else if (len != 1023)
		{
			close(fd);
			response += string(buf);
			return 200;
		}
		response += string(buf);
	}
	return 0;
}

DefaultMethodExecutor::~DefaultMethodExecutor() {}

int DefaultMethodExecutor::postMethod(const string &resourcePath, const string &request, string &response)
{
	static_cast<void>(response);
	if (step == STEP_OPEN_FILE)
	{
		fd = open(resourcePath.c_str(), O_WRONLY | O_NONBLOCK);
		if (fd == -1)
			return 500;
		step = STEP_IO_OPER;
		sh->change_events(fd, EVFILT_WRITE, EV_ADD, 0, 0, reinterpret_cast<void*>(client));
	}
	else if (step == STEP_IO_OPER)
	{
		ssize_t cnt = write(fd, resourcePath.c_str(), resourcePath.length());
		close(fd);
		if (cnt != static_cast<ssize_t>(resourcePath.length()))
			return 500;
		else if (request.length() == 0)
			return 204;
		return 201;
	}
	return 0;
}

int DefaultMethodExecutor::deleteMethod(const string &resourcePath) const
{
	if (remove(resourcePath.c_str()) != 0)
	{
		return 500;
	}
	return 204;
}

int DefaultMethodExecutor::putMethod(const string &resourcePath, const string &request, string &response)
{
	static_cast<void>(response);
		static_cast<void>(response);
	if (step == STEP_OPEN_FILE)
	{
		fd = open(resourcePath.c_str(), O_WRONLY | O_NONBLOCK);
		if (fd == -1)
			return 500;
		step = STEP_IO_OPER;
		sh->change_events(fd, EVFILT_WRITE, EV_ADD, 0, 0, reinterpret_cast<void*>(client));
	}
	else if (step == STEP_IO_OPER)
	{
		int cnt = write(fd, resourcePath.c_str(), resourcePath.length());
		close(fd);
		if (cnt != static_cast<ssize_t>(resourcePath.length()))
			return 500;
		else if (request.length() == 0)
			return 204;
		return 201;
	}
	return 0;
}

int DefaultMethodExecutor::headMethod(const string &resourcePath, string &response)
{
	if (step == STEP_OPEN_FILE)
	{
		fd = open(resourcePath.c_str(), O_RDONLY | O_NONBLOCK);
		if (fd == -1)
			return 500;
		step = STEP_IO_OPER;
		sh->change_events(fd, EVFILT_READ, EV_ADD, 0, 0, reinterpret_cast<void*>(client));
	}
	else if (step == STEP_IO_OPER)
	{
		char buf[1024];
		bzero(buf, 1024);
		int len = read(fd, buf, 1023);
		if (len < 0)
			return 500;
		else if (len != 1023)
			step = STEP_FINAL;
		response += string(buf);
	}
	else if (step == STEP_FINAL)
	{
		close(fd);
		return 200;
	}
	return 0;
}
