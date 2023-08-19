#include <iostream>
#include "ConfigParser.hpp"
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
		ConfigParser parser("tester.conf");
		ServerHandler sh(parser.get());
		sh.loop();
	}
	catch (const exception &e)
	{
		cerr << e.what() << endl;
		cerr << errno << ": " << strerror(errno) << '\n';
	}
	return 0;
}