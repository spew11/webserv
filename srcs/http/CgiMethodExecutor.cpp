#include "CgiMethodExecutor.hpp"

const int CgiMethodExecutor::READ = 0;
const int CgiMethodExecutor::WRITE = 1;
const int CgiMethodExecutor::STEP_FORK_PROC = 0x02;
const int CgiMethodExecutor::STEP_PROC_DIE = 0x04;
const int CgiMethodExecutor::STEP_PARENT_WRITE = 0x08;
const int CgiMethodExecutor::STEP_PARENT_READ = 0x10;
const int CgiMethodExecutor::STEP_CHILD = 0x11;

CgiMethodExecutor::CgiMethodExecutor(ServerHandler *sh, Client *client, char **cgiEnv)
	: sh(sh), cgiEnv(cgiEnv), write_buf_idx(0)
{
	stdin_fd = dup(STDIN_FILENO);
	stdout_fd = dup(STDOUT_FILENO);
	step = STEP_FORK_PROC;
	pid = -1;
	this->client = reinterpret_cast<void*>(client);
	parent_to_child_pipe[READ] = -1;
	parent_to_child_pipe[WRITE] = -1;	
	child_to_parent_pipe[READ] = -1;	
	child_to_parent_pipe[WRITE] = -1;	
}

CgiMethodExecutor::~CgiMethodExecutor()
{
	for (int i = 0; cgiEnv[i] != NULL; i++)
		delete cgiEnv[i];
	delete cgiEnv;
	close(stdin_fd);
	close(stdout_fd);
	if (parent_to_child_pipe[0] != -1)
		close(parent_to_child_pipe[0]);
	if (parent_to_child_pipe[1] != -1)
		close(parent_to_child_pipe[1]);
	if (child_to_parent_pipe[0] != -1)
		close(child_to_parent_pipe[0]);
	if (child_to_parent_pipe[1] != -1)
		close(child_to_parent_pipe[1]);
}

int CgiMethodExecutor::getMethod(const string &resourcePath, string &response, const int &exitCode)
{
	if (step == STEP_FORK_PROC)
	{
		if (pipe(child_to_parent_pipe) == -1)
			return 500;
		fcntl(child_to_parent_pipe[READ], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
		fcntl(child_to_parent_pipe[WRITE], F_SETFL, O_NONBLOCK, FD_CLOEXEC);

		pid = fork();
		if (pid == -1)
			return 500;
		else if (pid != 0)
		{
			close(child_to_parent_pipe[WRITE]);
			step = STEP_PROC_DIE;
			sh->change_events(pid, EVFILT_PROC, EV_ADD, NOTE_EXIT | NOTE_EXITSTATUS, 0, client);
			return 0;
		}
		else
			step = STEP_CHILD;
	}

	if (step == STEP_CHILD)
	{
		dup2(child_to_parent_pipe[WRITE], STDOUT_FILENO);
		close(STDIN_FILENO);
		close(child_to_parent_pipe[READ]);
		close(child_to_parent_pipe[WRITE]);

		char **args = new char *[2];
		args[0] = strdup(resourcePath.c_str());
		args[1] = NULL;
		close(STDIN_FILENO);
		execve(args[0], args, cgiEnv);
		exit(127);
	}

	if (step == STEP_PROC_DIE)
	{
		step = STEP_PARENT_READ;
		if (exitCode != 0)
			return 500;
		sh->change_events(child_to_parent_pipe[READ], EVFILT_READ, EV_ADD, 0, 0, client);
		return 0;
	}
	
	if (step == STEP_PARENT_READ)
	{
		int ret = read_from_pipe(child_to_parent_pipe[READ], response);
		if (ret != 200)
			return ret;
		return 200;
	}
	return 0;
}

int CgiMethodExecutor::postMethod(const string &resourcePath, const string &request, string &response, const int &exitCode)
{
	if (step == STEP_FORK_PROC)
	{
		if (pipe(parent_to_child_pipe) == -1)
			return 500;
		if (pipe(child_to_parent_pipe) == -1)
			return 500;

		fcntl(parent_to_child_pipe[READ], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
		fcntl(parent_to_child_pipe[WRITE], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
		fcntl(child_to_parent_pipe[READ], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
		fcntl(child_to_parent_pipe[WRITE], F_SETFL, O_NONBLOCK, FD_CLOEXEC);

		pid = fork();
		if (pid == -1)
			return 500;
		else if (pid != 0)
		{
			close(parent_to_child_pipe[READ]);
			close(child_to_parent_pipe[WRITE]);
			step = STEP_PARENT_WRITE;
			sh->change_events(parent_to_child_pipe[WRITE], EVFILT_WRITE, EV_ADD, 0, 0, client);
			return 0;
		}
		else
			step = STEP_CHILD;
	}

	if (step == STEP_CHILD)
	{
		dup2(parent_to_child_pipe[READ], STDIN_FILENO);
		dup2(child_to_parent_pipe[WRITE], STDOUT_FILENO);
		close(parent_to_child_pipe[READ]);
		close(parent_to_child_pipe[WRITE]);
		close(child_to_parent_pipe[READ]);
		close(child_to_parent_pipe[WRITE]);

		char **args = new char *[2];
		args[0] = strdup(resourcePath.c_str());
		args[1] = NULL;
		execve(resourcePath.c_str(), args, cgiEnv);
		exit(127);
	}

	if (step == STEP_PARENT_WRITE)
	{
		int ret = write_to_pipe(parent_to_child_pipe[WRITE], request);
		if (ret != 200)
			return ret;
		step = STEP_PROC_DIE;
		sh->change_events(pid, EVFILT_PROC, EV_ADD, NOTE_EXIT | NOTE_EXITSTATUS, 0, client);
		return 0;
	}
	
	if (step == STEP_PROC_DIE)
	{
		step = STEP_PARENT_READ;
		if (exitCode != 0)
			return 500;
		sh->change_events(child_to_parent_pipe[READ], EVFILT_READ, EV_ADD, 0, 0, client);
		return 0;
	}

	if (step == STEP_PARENT_READ)
	{
		int ret = read_from_pipe(child_to_parent_pipe[READ], response);
		if (ret != 200)
			return ret;
		return 200;
	}
	return 0;
}

int CgiMethodExecutor::deleteMethod(const string &resourcePath, const int &exitCode) const
{
	(void)resourcePath; (void)exitCode;
	return 501;
}

int CgiMethodExecutor::read_from_pipe(int &fd, string &body)
{
	char buf[1024];
	bzero(buf, 1024 * sizeof(char));

	ssize_t len = read(fd, buf, 1023);
	for (int i = 0; i < len; i++)
		body += buf[i];

	if (len < 0)
		return 500;
	else if (len < 1023)
	{
		close(fd);
		write_buf_idx = 0;
		return 200;
	}
	return 0;
}

int CgiMethodExecutor::write_to_pipe(int &fd, const string &body)
{
	char buf[1024];
	bzero(buf, sizeof(char) * 1024);
	size_t i = 0;
	for (; i < 1024 && i + write_buf_idx < body.size(); i++)
		buf[i] = body[i + write_buf_idx];

	ssize_t cnt = write(fd, buf, i);
	if (cnt != static_cast<ssize_t>(i))
		return 500;
	if (i + write_buf_idx == body.size())
	{
		close(fd);
		write_buf_idx = 0;
		return 200;
	}
	write_buf_idx += i;
	return 0;
}

int CgiMethodExecutor::putMethod(const string &resourcePath, const string &request, string &response, const int &exitCode)
{
	return postMethod(resourcePath, request, response, exitCode);
}

int CgiMethodExecutor::headMethod(const string &resourcePath, string &response, const int &exitCode)
{
	return getMethod(resourcePath, response, exitCode);
}