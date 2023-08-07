#include <iostream>
#include "ConfigParser.hpp"
#include "ServerHandler.hpp"
#include "HttpResponseBuilder.hpp"
#include "IMethodExecutor.hpp"
#include "DefaultMethodExecutor.hpp"
#include "CgiMethodExecutor.hpp"
#include "WebservValues.hpp"
#include "ServerConfig.hpp"
int main(void)
{
	try
	{
		ConfigParser parser("test");
		ServerHandler sh(parser.get());
		sh.loop();
	}
	catch(const std::exception& e)
	{
		std::cerr << errno << ": " << strerror(errno) << '\n';
	}
	return 0;

	// ResponseBuilder 사용 예상 예시
	
	// WebservValues webservValues;
	// string request = "GET /bin/ HTTP/1.1\r\n";
	// HttpResponseBuilder rhb; //빌더 생성
	// ServerConfig serverConfig;

	// HttpResponseBuilder hrb;
	// hrb.initiate(request, webservValues, serverConfig.getLocationMap()); //초기화
	// string host = hrb.getRequestMessage().getHeader("host"); //host 확인하는 것
	// IMethodExecutor *methodExecutor;
	// while (hrb.getNeedMoreMessageFlag() != false)
	// {
	// 	hrb.addRequestMessage(request); //스트링 타입 계속 넣기
	// }
    // if (hrb.getNeedCgiFlag()) {
	// 	methodExecutor = new CgiMethodExecutor(cgiEnv);
	// }
	// else {
	// 	methodExecutor = new DefaultMethodExecutor();
	// }
	// hrb.build(*methodExecutor); //메소드 실행 및 헤더 알아서 붙여줌
	// string response = hrb.getResponseMessage().toString();
}