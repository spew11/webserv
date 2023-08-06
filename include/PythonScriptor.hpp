#ifndef PYTHON_SCRIPTOR_HPP
# define PYTHON_SCRIPTOR_HPP
# include <iostream>
# include <string>
# include <fstream>
# include <unistd.h>
# include "ICgiScriptor.hpp"

using namespace std;

class PythonScriptor : ICgiScriptor
{
private:
	char **env;
	
public:
	PythonScriptor(char **env);
	~PythonScriptor();
	void run(const string &scriptFile);
};

#endif