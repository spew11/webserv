#include <iostream>
#include "ServerHandler.hpp"

int main(void)
{
	try
	{
		/* code */
		ServerHandler sh;
		sh.loop();
	}
	catch(const std::exception& e)
	{
		std::cerr << errno << ": " << strerror(errno) << '\n';
	}
	
	return 0;
}