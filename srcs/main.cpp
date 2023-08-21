#include <iostream>
#include "ServerHandler.hpp"
#include "HttpResponseBuilder.hpp"
#include "IMethodExecutor.hpp"
#include "DefaultMethodExecutor.hpp"
#include "CgiMethodExecutor.hpp"
#include "WebservValues.hpp"
#include "ServerConfig.hpp"
#include "Config.hpp"

using namespace std;

int main(void)
{
	try
	{
		Config config("tester.conf");
		ServerHandler sh(&config);
		sh.loop();
	}
	catch (const exception &e)
	{
		cerr << e.what() << endl;
		cerr << errno << ": " << strerror(errno) << '\n';
	}
	return 0;
}
