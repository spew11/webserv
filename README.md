# HTTP Web Server
## Index
- [Introduction](#Introduction)
  - [Why We Built This](#Why-We-Built-This)
  - [Role](#Role)
  - [Objective](#Objective)
- [Features](#Features)
- [Requirements](#Requirements)
- [Installation & Setup](#Installation--Setup)
- [Usage](#Usage)
  - [Tests](#Tests)
  - [Set Configuration](#Set-Configuration)
    - [Config file name convention](#Config-file-name-convention)
    - [Config file directives](#Config-file-directives)
    - [Example of the default configuration file 'tester.conf'](#Example-of-the-default-configuration-file-testerconf)
- [What I Learned](#What-I-Learned)
  
## Introduction
Server는 설정 파일에 명시된 파라미터를 기반으로 초기화됩니다.  
HTTP 요청을 수신하여 해당 요청에 대한 적절한 HTTP 응답을 생성하고 반환합니다.

### Why We Built This
웹 개발을 목표를 하면서, 웹 서버의 동작 방식 및 HTTP프로토콜을 이론으로만 학습하고 넘어가기에는 아쉬움이 있었기에,  
웹 서버를 직접 구현하면서 원리를 정확히 이해해 보고자 했습니다.

### Role
- @izone00: Server configuration 파싱
- @ssoyeong-lee: Server socket programming 구현
- @spew11: HTTP protocol 구현

### Objective
- nginx를 차용한 configuration 파싱
- IO multi-plexing 기반의 서버 구현
  - 성능 테스트에서 가용성 99.5% 확보할 것
- HTTP/1.1 버전 프로토콜 구현

## Features
- 5가지 HTTP 메서드를 제공합니다.
  - GET : 서버에서 클라이언트로 지정한 리소스를 보냅니다.
  - PUT : 클라이언트에서 서버로 보낸 데이터를 지정한 이름의 리소스로 저장합니다.
  - DELETE : 지정한 리소스를 서버에서 삭제합니다.
  - POST : PUT 메서드와 동일한 처리를 합니다. 
  - HEAD : 서버에서 클라이언트로 지정한 리소스에 대한 응답에서, HTTP 헤더 부분만 보냅니다.
- 동적 웹 페이지의 생성과 처리를 수행합니다.
  - python 지원

## Requirements
- Language : c++
- Libraries : c++98 standard library
- OS: MAC OS
  
## Installation & Setup
```
git clone https://github.com/spew11/webserv.git
&& make
```

## Usage

### Tests
```bash
./webserve tester.conf
```

### Set Configuration

#### Config file name convention
`[이름].conf`

#### Config file directives
아래의 디렉티브(directives)를 이용해 웹 서버의 동작을 제어할 수 있습니다.
- 'server' : 서버에서 하나의 서버 블록을 정의합니다.  
     이 블록 내에는 해당 서버에 대한 여러 설정과 지시어들이 위치하게 됩니다.
     하나의 nginx.conf 파일 내에 여러 개의 server 블록을 정의할 수 있고,
     각각의 server 블록은 다른 포트 또는 도메인에 대한 설정을 가질 수 있습니다.
  ```nginx
  server {
    listen 127.0.0.1:8080;
    server_name example.com www.example.com;

    location / {
        root /var/www/example.com;
        index index.html index.htm;
    }
  }
  ```
  
- 'listen' : 서버가 어떤 IP 주소와 포트에서 클라이언트의 요청을 대기할 것인지 설정합니다.
     ```nginx 
     listen 127.0.0.1:8088;
     ```
- 'server_name' : 서버의 도메인 이름을 지정합니다.
   ```nginx
   server_name localhost;
   ```
- 'location' : 특정 URL 패턴에 대한 요청을 어떻게 처리할 것 인지를 지정합니다.  
   location지시어에서는 접두사 매칭(Prefix Mathes('/'))만을 지원합니다.   
   ```nginx
   location / {
      index index.html;
      accept_method GET;
      autoindex on;
   }
   ```
- 'root' : 웹 서버의 루트 디렉토리를 지정합니다.
   ```nginx
   root /var/www/html;
   ```
- 'index' : 디렉토리의 기본 파일을 설정합니다.
   ```nginx
   index index.html index.htm;
   ```
- 'accept_method' : 특정 HTTP 메서드를 허용합니다. accep_method에 해당하지 않는 메서드는 모두 제한됩니다.
  ```nginx
  location / {
      index index.html;
      accept_method GET;
      autoindex on;
  }
  ```
- 'error_page' : 특정 HTTP 상태 코드가 반환될 때 사용자에게 보여줄 페이지를 설정합니다.
   ```nginx
   error_page 404 /404.html;
   ```
- 'autoindex' : 디렉토리의 목록을 자동으로 생성합니다.
   ```nginx
   location /files/ {
     autoindex on;
   }
   ```
- 'client_max_body_size' : 클라이언트로부터 전달받을 수 있는 요청 본문의 최대 크기를 제한합니다.
  ```nginx
  location /upload/ {
    client_max_body_size 1000;
  }
  ```
- 'cgi' : CGI(Common Gate Interface)를 통해 CGI스트립트를 실행할 수 있습니다.
  현재는 python만을 지원합니다. 
  ```nginx
  location /cgi-bin/ {
        cgi py;
  }
  ```
- 'return' : 특정 URL로 들어오는 요청을 다른 URL로 전달합니다.
  ```nginx
  location /old-location {
    return 301 /new-location;
  }
  ```
- 'types' : 파일 확장자와 해당 파일이 어떤 MIME 타입으로 처리될 것인지를 지정합니다.  
    이 설정은 HTTP 응답 헤더의 'Content-Type' 필드 값을 결정하는 데 사용됩니다.
  ```nginx
  types {
    text/html    html htm;
    image/jpeg   jpg jpeg;
    application/javascript js;
  }
  ```
- 'cgi-params' : CGI스크립트가 동작할 때 필요한 정보들을 제공합니다.
  ```nginx
  cgi_params {
        PARAM_NAME   $variable_name;
        ...
  }
  ```
  
#### Example of the default configuration file 'tester.conf'
```nginx
server  
{
    listen 127.0.0.1:8088;

    server_name localhost;
    index index.html;
    error_page 404 /customErrPage/404_error.html;

    location / {
        index index.html;
        accept_method GET;
        autoindex on;
    }

    location /put_test {
        accept_method PUT;
        client_max_body_size 1000000000;
    }

    location /post_body {
        accept_method POST DELETE;
        client_max_body_size 1000000000;
    }

    location /directory/ {
        accept_method GET;
        root YoupiBanane;
        index youpi.bad_extension;
    }

    location /cgi-bin/ {
        cgi py;
        index index.py;
        accept_method GET POST;
        client_max_body_size 1000000000;
    }
    location /customErrPage/ {
        root Custom;
        accept_method GET;
        autoindex on;
    }

    types {
        text/html                             html htm shtml;
        text/css                              css;
        text/xml                              xml rss;
        image/gif                             gif;
        image/jpeg                            jpeg jpg;
        application/x-javascript              js;
        text/plain                            txt;
        text/x-component                      htc;
        text/mathml                           mml;
        image/png                             png;
        image/x-icon                          ico;
        image/x-jng                           jng;
        image/vnd.wap.wbmp                    wbmp;
        application/java-archive              jar war ear;
        application/mac-binhex40              hqx;
        application/pdf                       pdf;
        application/x-cocoa                   cco;
        application/x-java-archive-diff       jardiff;
        application/x-java-jnlp-file          jnlp;
        application/x-makeself                run;
        application/x-perl                    pl pm;
        application/x-pilot                   prc pdb;
        application/x-rar-compressed          rar;
        application/x-redhat-package-manager  rpm;
        application/x-sea                     sea;
        application/x-shockwave-flash         swf;
        application/x-stuffit                 sit;
        application/x-tcl                     tcl tk;
        application/x-x509-ca-cert            der pem crt;
        application/x-xpinstall               xpi;
        application/zip                       zip;
        application/octet-stream              deb;
        application/octet-stream              bin exe dll;
        application/octet-stream              dmg;
        application/octet-stream              eot;
        application/octet-stream              iso img;
        application/octet-stream              msi msp msm;
        audio/mpeg                            mp3;
        audio/x-realaudio                     ra;
        video/mpeg                            mpeg mpg;
        video/quicktime                       mov;
        video/x-flv                           flv;
        video/x-msvideo                       avi;
        video/x-ms-wmv                        wmv;
        video/x-ms-asf                        asx asf;
        video/x-mng                           mng;
    }
    
    cgi_params {
            QUERY_STRING            $query_string;
            REQUEST_METHOD          $request_method;
            CONTENT_TYPE            $content_type;
            CONTENT_LENGTH          $content_length;
    
            SCRIPT_FILENAME         $document_root$fastcgi_script_name;
            SCRIPT_NAME             $fastcgi_script_name;
            PATH_INFO               $fastcgi_path_info;
            PATH_TRANSLATED         $document_root$fastcgi_path_info;
            REQUEST_URI             $request_uri;
            DOCUMENT_URI            $document_uri;
            DOCUMENT_ROOT           $document_root;
            SERVER_PROTOCOL         $server_protocol;
    
            GATEWAY_INTERFACE       CGI/1.1;
            SERVER_SOFTWARE         nginx/$nginx_version;
    
            REMOTE_ADDR             $remote_addr;
            REMOTE_PORT             $remote_port;
            SERVER_ADDR             $server_addr;
            SERVER_PORT             $server_port;
            SERVER_NAME             $server_name;
    
            HTTPS                   $https;
    	  }
}
```
## What I Learned
웹 서버를 위해 사용되었던 socket IPC, IO multi-plexing 기술의 이론학습 뿐만 아니라, 실제 동작 원리를 익힐 수 있었습니다.
특히 HTTP request 파싱, HTTP 헤더 구현, HTTP 응답 메시지 생성, server configuration directives 구현등을 통해서    
HTTP 프로토콜이 구체적으로 와닿는 계기가 되었습니다. 
