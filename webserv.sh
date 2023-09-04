#!/bin/bash

# 지정된 파일 이름
filename="./tester.conf"

# 파일이 존재하는지 확인
if [ ! -f "$filename" ]; then
    echo "File $filename not found!"
    exit 1
fi

# 현재의 포트 번호를 추출 (첫 번째로 찾은 것만 사용)
current_port=$(grep -m 1 -o '127\.0\.0\.1:[0-9]\+' "$filename" | awk -F: '{ print $2 }')

# 만약 포트 번호가 없다면 종료
if [ -z "$current_port" ]; then
    echo "No port number found in $filename."
    exit 1
fi

# 포트 번호를 8080에서 8090 사이로 순환 증가
new_port=$(((current_port - 8080 + 1) % 11 + 8080))

# sed를 사용하여 파일 내용을 변경 (macOS와 Linux에서 동작)
if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS
    sed -i '' "s/127.0.0.1:$current_port/127.0.0.1:$new_port/g" "$filename"
else
    # Linux
    sed -i "s/127.0.0.1:$current_port/127.0.0.1:$new_port/g" "$filename"
fi

#echo "Port changed from $current_port to $new_port in $filename"
./webserv tester.conf