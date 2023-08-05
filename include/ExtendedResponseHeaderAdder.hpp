#ifndef EXTENDED_RESPONSE_HEADER_ADDER_HPP
# define EXTENDED_RESPONSE_HEADER_ADDER_HPP
# include "ResponseHeaderAdder.hpp"
# include <ctime>

class ExtendedResponseHeaderAdder : ResponseHeaderAdder
{
    public:
        ExtendedResponseHeaderAdder(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessage), \
            const LocationConfig & locationConfig, const string & requestBody);
        void executeAll();
        void addDateHeader();

        /* 아래 response header type 추가 여부는 선택사항임

        void addServerHeader();
            서버의 이름과 버전 정보를 응답에 포함한다.

        void addContentEncodingHeader();
            응답 본문에 적용된 인코딩 방식을 표시한다.

        void addContentLanguageHeader();
            응답 콘텐츠의 자연어를 나타낸다.

        void addContentDisposition();
            응답 콘텐츠를 표시하는 방법에 대한 지시를 제공한다.

        void addAccessControlAllowOrigin();
            교차 출처 요청에 대한 리소스 접근을 허용하는 출처를 정의한다.

        void addXPoweredByHeader();
            서버가 요청을 처리하는데 사용되는 기술이나 프레임워크를 표시한다.

        void addRetryAfterHeader();
            응답을 받은 후 클라이언트가 후속 요청을 하기 전에 기다려야 하는 시간을 지정한다.

        void ContentSecurityPolicy();
            웹 페이지에서 콘텐츠를 로드하는 정책을 정의한다.

        void addETagHeader();
            1. 어떤 콘텐츠에 대응하는 해쉬값을 응답에 같이 넘겨주는 것임. (해시함수 구현해야됌)
            2. 요청헤더타입이 'if-None-Match'(기존의 콘텐츠와 다를 떄 메소드 동작) 혹은 'if-Match'(기존의 콘텐츠와 같을때 메소드 동작)일 때 붙여줄 수 있음.
            3. 설령 나의 서버는 ETag 헤더를 제공하지 않는데 클라이언트에서 if-None-Match 타입의 요청 헤더를 전달한다면 그냥 무시하면 됌.

        void addLastModifiedHeader();
            1. 요청헤더타입이 'if-Modified-Since'(특정 시간 이후로 변경되었다면 마지막 수정 시간 반영)일 때 붙여줄 수 있음
            2. 리소스에 대한 마지막 수정 시간에 대한 정보가 없다면 그냥 무시하면 됌.
            3. 선택사항임

        void addLocationHeader();
            1. 응답될 상태코드가 3xx(리다이렉트) 거나 201(created) 일 때 붙여준다.
            2. root에 Locaiton에 명시된 새로운 url을 붙여 반환한다. (ex. Location: https://example.com/new-page)
            3. 클라이언트는 이렇게 반환된 응답 메시지를 읽고 알게된 url에 대해서 다시 요청을 보낸다.(웹브라우저가 알아서 해줌)

        void addAllowHeader();
            1. request http method가 OPTIONS로 들어올 때 붙여준다.
            2. 리소스에서 허용되는 HTTP 메서드를 표시한다. (ex. Allow: GET, POST, HEAD, OPTIONS)
        */
};
#endif