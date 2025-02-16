# Webserv 🚀
  
## 🌟 소개
✨ **IO multi-plexing 기반 HTTP Web Server를 구현한 프로젝트입니다.** ✨  

### 📊 성능 비교 (webserv vs nginx vs apache)
  
  |requests| relative throughput|
  |-------------------|--------------------- |
  |![웹서브측정1](https://github.com/user-attachments/assets/923d36e9-d77b-4bbc-b578-e860f3d67839)|![웹서브측정2](https://github.com/user-attachments/assets/e5c832ca-917f-4683-90bd-d9129744ac77)|
  |[차트 1] websev와 nginx 및 apache의 초당 처리량 비교 | [차트 2] 각 서버의 초당 처리량 상대적 비교 (webserv: 100% 기준) |

  **webserv는 nginx(#workers=1) 대비 약 53%의 처리량을 보이고 있습니다.**<br>
  동시 요청 수(num_clients)가 늘어났음에도 처리량이 크게 증가하지 않는 한계가 있으나, 이는 nginx(#workers=1)에서도 동일하게 발견되는 경향으로, multi-process를 사용할 경우 해소될 수 있을 것으로 생각됩니다.

## 🛠 실험 환경

### 🔹 서버 환경
- 🐳 **Nginx**: Docker (`nginx:stable-alpine3.20`)  
  - **CPU:** 4  
  - **RAM:** 4GB  
- 🐳 **Apache**: Docker (`httpd:alpine3.21`)  
  - **CPU:** 4  
  - **RAM:** 4GB  
- 💻 **Webserv**: local 실행 (MacBook Air M3)  

### 🔹 클라이언트 환경
- 🐳 **Client**: Docker (`ubuntu:22.04`)  
  - **CPU:** 4  
  - **RAM:** 4GB
- 🛠️ **테스트 도구:** [`wrk`](https://github.com/wg/wrk)
- ⏳ **테스트 지속 시간**: 60초
  
## 🗓️ 프로젝트 진행 기간  
📅 **설계 및 주요 기능 구현:** `2023.06.20 ~ 2023.08.13`

## 👥 팀원
<table>
  <tr>
    <td align="center">
      <a href="https://github.com/spew11">
        <img src="https://github.com/spew11.png" width="100" height="100"><br>
        <b>spew11</b>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/izone00">
        <img src="https://github.com/izone00.png" width="100" height="100"><br>
        <b>izone</b>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/ssoyeong-lee">
        <img src="https://github.com/ssoyeong-lee.png" width="100" height="100"><br>
        <b>ssoyeong-lee</b>
      </a>
    </td>
  </tr>
  <tr>
    <td valign="top">
      🔹 <b>HTTP 프로토콜 구현</b> <br>
      🔹 <b>socket programming 구현</b> <br>
    </td>
    <td valign="top">
      🔹 <b>configration 구현</b> <br>
      🔹 <b>socket programming 구현</b> <br>
    </td>
    <td valign="top">
      🔹 <b>socket programming 구현</b> <br>
      🔹 <b>HTTP 프로토콜 구현</b> <br>
    </td>
  </tr>
</table>

## 🍰 제공 기능
- 🍭 **nginx 스타일의 설정 파일 및 디렉티브 지원**
- 📡 **HTTP/1.1 지원 및 5가지 HTTP 메서드 지원 (GET, PUT, DELETE, POST, HEAD)** 📨  
- 🐍 **Python 기반 동적 웹페이지 처리 지원**
- 🎈 **99.7%의 가용성 보장**

## 🛠️ 환경 
- 📌 **Language:** C++  
- 📚 **Libraries:** C++98 Standard Library  
- 💻 **OS:** macOS  

## 🎨 설치 방법
```bash
git clone https://github.com/spew11/webserv.git && make
```
## 🎮 실행 방법

```bash
./webserve tester.conf
```
