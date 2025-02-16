# Webserv 🚀
  
## 🌟 소개
✨ **IO multi-plexing 기반 HTTP Web Server를 구현한 프로젝트입니다.** ✨  

### 📊 성능 비교 (ours vs nginx vs apache)
  
  |requests| relative throughput|
  |-------------------|--------------------- |
  |![웹서브측정1](https://github.com/user-attachments/assets/923d36e9-d77b-4bbc-b578-e860f3d67839)|![웹서브측정2](https://github.com/user-attachments/assets/e5c832ca-917f-4683-90bd-d9129744ac77)|
  |[표 1] websev와 nginx 및 apache의 초당 처리량 비교 | [표 2] 각 서버의 초당 처리량 상대적 비교 (werserv: 100% 기준) |

  **webserv는 nginx(#workers=1) 대비 약 53%의 처리량을 보이고 있습니다.**<br>
  동시 요청 수(num_clients)가 늘어났음에도 처리량이 크게 증가하지 않는 한계가 있으나, 이는 nginx(#workers=1)에서도 동일하게 발견되는 경향으로, multi-process를 사용할 경우 해소될 수 있을 것으로 생각됩니다.

## 🍰 특징
- 🍭 **nginx 스타일의 설정 파일 및 디렉티브 지원**
- 📡 **HTTP/1.1 지원 및 5가지 HTTP 메서드 지원 (GET, PUT, DELETE, POST, HEAD)** 📨  
- 🐍 **Python 기반 동적 웹페이지 처리 지원**
- 🎈 **99.7%의 가용성 보장**

## 🛠️ 환경 
- 📌 **Language:** C++  
- 📚 **Libraries:** C++98 Standard Library  
- 💻 **OS:** macOS  


## 🎨 설치
```bash
git clone https://github.com/spew11/webserv.git && make
```

## 🎮 실행

```bash
./webserve tester.conf
```
