# Webserv 🚀
  
## 🌟 소개
✨ **IO multi-plexing 기반 HTTP Web Server를 구현한 프로젝트입니다.** ✨  

## 🍰 제공 기능
- 🍭 **nginx 스타일의 설정 파일 및 디렉티브 지원**
- 📡 **HTTP/1.1 지원 및 5가지 HTTP 메서드 지원 (GET, PUT, DELETE, POST, HEAD)** 📨  
- 🐍 **Python 기반 동적 웹페이지 처리 지원**
- 🎈 **99.7%의 가용성 보장**

## 📊 성능 비교 (webserv vs nginx vs apache)
  
  |requests| relative throughput|
  |-------------------|--------------------- |
  |![웹서브측정1](https://github.com/user-attachments/assets/923d36e9-d77b-4bbc-b578-e860f3d67839)|![웹서브측정2](https://github.com/user-attachments/assets/e5c832ca-917f-4683-90bd-d9129744ac77)|
  |[차트 1] websev와 nginx 및 apache의 초당 처리량 비교 | [차트 2] 각 서버의 초당 처리량 상대적 비교 (webserv: 100% 기준) |

  **webserv는 nginx(#workers=1) 대비 약 53%의 처리량을 보이고 있습니다.**<br>
  동시 요청 수(num_clients)가 늘어났음에도 처리량이 크게 증가하지 않는 한계가 있으나, 이는 nginx(#workers=1)에서도 동일하게 발견되는 경향으로, multi-process를 사용할 경우 해소될 수 있을 것으로 생각됩니다.

## 🚀 동시성 처리 방법: I/O 멀티플렉싱
**동시성 처리는 웹서버의 성능을 결정하는데 가장 중요한 요인입니다.
대표적인 동시성 처리 방법은 크게 2가지가 있습니다.**
1. multi-process / multi-thread (Apache)
2. single-thread 기반 IO 멀티플렉싱 (Nginx)

**본 프로젝트에서는** ${\color{#ff0000}Apache와\ Nginx의\ 성능을\ 비교}$**한 뒤, 더 좋은 성능을 보인 Nginx에서 채택한 동시성 처리 방법인** ${\color{#ff0000}I/O\ 멀티플렉싱}$**을 채택했습니다.**

### 1️⃣ multi-process / multi-thread(Apache)
<img width="1126" alt="스크린샷 2025-02-21 오후 8 44 01" src="https://github.com/user-attachments/assets/f9a8a182-bca3-4779-9721-fc10b09607ed" />

- ${\textcolor{#ff0000}{다수의\ thread를\ 운용}}$, **각 thread는 한 번 connection을 맺은 client와의 연결이 종료될 때 까지 지속 담당하며, 다른 client의 요청은 대응하지 않습니다.**
- **thread는 client가 다음 요청을 보내기까지의 시간을 wait해야 한다는 점에서 비효율적입니다.** ${\color{#ff0000}(blocking)}$
- **동시에 요청을 처리하기 위해서는** ${\color{#ff0000}1개의\ client\ 당\ 1개\ thread가\ 필요}$**하다는 점에서 비효율적입니다.**
- ${\color{#ff0000}context\ switching이\ 빈번하게\ 발생}$**한다는 점에서 비효율적입니다.**

### 2️⃣ IO multi-plexing (Nginx)
<img width="1099" alt="스크린샷 2025-02-21 오후 8 39 32" src="https://github.com/user-attachments/assets/1dfa4ab8-4cb6-4137-960e-f40bd6fd20c4" />

- ${\textcolor{#ff0000}{1개의\ thread}}$**가 event queue를 watch하며 event가 발생하면 대응합니다.** ${\textcolor{#ff0000}{(event-driven)}}$
- **한 client와의 connection에 묶여 다음 요청을 wait 하지 않고, 다른 client의 요청들을 수행합니다** ${\textcolor{#ff0000}{(non-blocking)}}$
- ${\textcolor{#ff0000}{1개의\ thread로\ 수많은\ client의\ 요청을\ 대응할\ 수\ 있다}}$**는 점에서 효율적입니다.**
- **단일 thread이므로 내부에서** ${\textcolor{#ff0000}{context-switching이\ 발생하지\ 않는다}}$**는 점에서 효율적입니다.**

### 
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
