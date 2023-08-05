#include "CgiMethodExecutor.hpp"

CgiMethodExecutor::CgiMethodExecutor(ICgiScriptor *cgiScriptor) {}

int CgiMethodExecutor::getMethod(const string &resourcePath, string &content) const
{
    // resourcePath가 가르키는 파일을 바탕으로 cgiScriptor 실행하고 결과값은 content에 저장...이런식으로
    // 그리고 실행결과에 따른 응답 status 코드 리턴하기.. 
    return 200;
}

int CgiMethodExecutor::postMethod(const string &pathToSave, const string &content) const
{
    return 200;
}

int CgiMethodExecutor::deleteMethod(const string & resourcePath) const
{
    return 200;
}