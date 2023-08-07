#include "PythonScriptor.hpp"

PythonScriptor::PythonScriptor(char **env): env(env) {}
PythonScriptor::~PythonScriptor()
{
	delete env;
}

void PythonScriptor::run(const string &scriptFile)
{
	//find python3 path
	//create args
	char *cmd = "python3";
	char *fileName = new char[scriptFile.size() + 1];
	strlcpy(fileName, scriptFile.c_str(), scriptFile.size() + 1);
	char *args[] = {cmd, fileName, NULL};
	int ret = execve(cmd, args, env);k
	if (ret == -1)
		exit(1);
}