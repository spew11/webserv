#include "CgiMethodExecutor.hpp"

CgiMethodExecutor::CgiMethodExecutor(ICgiScriptor *cgiScriptor): cgiScriptor(cgiScriptor)
{
	stdin_fd = dup(STDIN_FILENO);
	stdout_fd = dup(STDOUT_FILENO);
}

int CgiMethodExecutor::getMethod(const string &resourcePath, string &content) const
{
	int child_to_parent_pipe[2];

	if (pipe(child_to_parent_pipe) == -1)
		throw std::exception();
	int pid = fork();
	if (pid == -1)
	{
		throw std::exception();
	}
	else if (pid  == 0)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		dup2(child_to_parent_pipe[WRITE], STDOUT_FILENO);
		close(child_to_parent_pipe[READ]);
		close(child_to_parent_pipe[WRITE]);
		cgiScriptor->run(resourcePath); //execve (scriptor 안ㅇ에 env 존재);
	}
	else
	{
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
		dup2(child_to_parent_pipe[READ], STDIN_FILENO);
		close(child_to_parent_pipe[WRITE]);
		close(child_to_parent_pipe[READ]);
		int exit_code;
		waitpid(pid, &exit_code, WUNTRACED);
		content = read_from_pipe();
		dup2(stdin_fd, STDIN_FILENO);
		if (exit_code == 0)
			return 200;
		else
			return 400;
	}
}

int CgiMethodExecutor::postMethod(const string &pathToSave, const string &content) const
{
	int parent_to_child_pipe[2];
	int child_to_parent_pipe[2];
	
	if (pipe(parent_to_child_pipe) == -1)
		throw std::exception();
	if (pipe(child_to_parent_pipe) == -1)
		throw std::exception();
	int pid = fork();
	if (pid == -1)
	{
		throw std::exception();
	}
	else if (pid  == 0)
	{
		dup2(parent_to_child_pipe[READ], STDIN_FILENO);
		dup2(child_to_parent_pipe[WRITE], STDOUT_FILENO);
		close(parent_to_child_pipe[READ]);
		close(parent_to_child_pipe[WRITE]);
		close(child_to_parent_pipe[READ]);
		close(child_to_parent_pipe[WRITE]);
		cgiScriptor->run(pathToSave);
	}
	else
	{
		dup2(child_to_parent_pipe[READ], STDIN_FILENO);
		dup2(parent_to_child_pipe[WRITE], STDOUT_FILENO);
		close(parent_to_child_pipe[READ]);
		close(parent_to_child_pipe[WRITE]);
		close(child_to_parent_pipe[READ]);
		close(child_to_parent_pipe[WRITE]);

		write_to_pipe(?); //where is body...?

		int exit_code;
		waitpid(pid, &exit_code, WUNTRACED);
		content = read_from_pipe();
		dup2(stdin_fd, STDIN_FILENO);
		dup2(stdout_fd, STDOUT_FILENO);
		if (exit_code == 0)
			return 200;
		else
			return 400;
	}
}

int CgiMethodExecutor::deleteMethod(const string & resourcePath) const
{
	return 501; //Not Implement
}

string CgiMethodExecutor::read_from_pipe()
{
	char buf[1024];
	bzero(buf, 1024 * sizeof(char));
	
	int ret;
	string res;
	while ((ret = read(STDIN_FILENO, buf, 1024)) > 0)
	{
		res += string(buf);
		bzero(buf, 1024 * sizeof(char));
	}
	if (ret == -1)
		throw std::exception();
	return res;
}

void CgiMethodExecutor::write_to_pipe(string body)
{
	if (write(STDOUT_FILENO, body.c_str(), body.size() + 1) == -1)
		throw std::exception();
}