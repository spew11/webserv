#include "ResponseHeaderAdder.hpp"
#include "Config.hpp" //이거 약간 고민, 생성자로 넣어주는 것보다 걍 선언해서 쓰는게 나중에 덜 고치지 않을까,,

// ReponseHeaderAdder::ReponseHeaderAdder(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessage) :
//     requestMessage(requestMessage), responseMessage(responseMessage){}

virtual void ReponseHeaderAdder::execute(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessag)
{
    // 캐시는 구현안하니까 주석처리 될듯..그리고 지시어 넘겨줘야됌 함수에 나중에!
    if (requestMessage.getHeader("Cache-Control") != "") {
        addCacheCotrolHeader(requestMessage, responseMessag);
    }
    if (requestMessage.getHeader("if-None-Match") != "") {
        addETagHeader(requestMessage, responseMessag);
    } 
    if (requestMessage.getHeader("if-Modified-Since") != "") {
        addLastModifiedHeader(requestMessage, responseMessag);
    }
    if (client가 요청한 url이 다른 location으로 redirect 됐는지 확인) {
        addLocationHeader(requestMessage, responseMessag, 새로운 경로);
    }
    if (requestMessage.getMethod() == "OPTIONS") {
        addAllowHeader(requestMessage, responseMessag)   
        //Allow: GET, POST, HEAD, OPTIONS 이런식으로 보여줘야됌     
    }
    addContentTypeHeader(requestMessage, responseMessag);
    addContentLengthHeader(requestMessage, responseMessag);
}
//필요한 config지시어는 Cache-control, locationMap, accpet_method, Content-type, 
void ReponseHeaderAdder::addCacheCotrolHeader(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessag)
{
    if (requestMessage.getMethod() == "GET") {
        // GET 요청에 대한 Cache-Control 지시어 추가하기
        responseMessage.addHeader("Cache-Control", "public, max-age=3600") // 여기서 value값은 그냥 예시임. 아마도 nginx config에서 가져와야겠지
    }
    else {
        responseMessag.addHeader("Cache-Control", "no-cache, no-store, must-revalidat");
    }
}

void ReponseHeaderAdder::addContentTypeHeader(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessag)
{

}

void ReponseHeaderAdder::addContentLengthHeader(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessag);
void ReponseHeaderAdder::addETagHeader(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessag);
void ReponseHeaderAdder::addLastModifiedHeader(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessag);
void ReponseHeaderAdder::addLocationHeader(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessag);
void ReponseHeaderAdder::addAllowHeader(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessag);