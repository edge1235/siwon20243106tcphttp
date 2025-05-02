#include <stdio.h>      // 표준 입출력 함수 사용 (printf, perror 등)
#include <stdlib.h>     // 표준 라이브러리 함수 사용 (exit 등)
#include <string.h>     // 문자열 처리 함수 사용 (strlen 등)
#include <unistd.h>     // 유닉스 시스템 호출 함수 사용 (close, read, write 등)
#include <arpa/inet.h>  // 네트워크 주소 변환 및 소켓 관련 함수 사용 (socket, bind, connect 등)

#define SERVER_IP "127.0.0.1"    // 서버 IP 주소를 localhost로 지정
#define SERVER_PORT 8080         // 서버 포트 번호를 8080으로 지정
#define BUFFER_SIZE 4096         // 데이터 송수신에 사용할 버퍼 크기를 4096바이트로 지정

// 서버로 HTTP 요청을 보내고 응답을 출력하는 함수
void send_request(const char *request)
{
    int sock;                        // 서버와 통신할 소켓 디스크립터
    struct sockaddr_in server_addr;   // 서버 주소 정보를 저장할 구조체
    char buffer[BUFFER_SIZE];         // 서버 응답을 받을 버퍼
    int bytes_read;                   // 읽은 바이트 수

    // 1. TCP 소켓 생성
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)  // 소켓 생성 실패 시
    {
        perror("socket failed");  // 에러 출력
        exit(EXIT_FAILURE);       // 프로그램 종료
    }

    // 2. 서버 주소 설정
    server_addr.sin_family = AF_INET;               // IPv4 사용
    server_addr.sin_port = htons(SERVER_PORT);       // 포트 번호를 네트워크 바이트 오더로 변환하여 저장
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0)
    {
        // IP 주소를 네트워크 주소로 변환 실패 시
        perror("inet_pton failed");
        close(sock);              // 소켓 닫기
        exit(EXIT_FAILURE);       // 프로그램 종료
    }

    // 3. 서버에 연결 시도
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect failed"); // 연결 실패 시 에러 출력
        close(sock);              // 소켓 닫기
        exit(EXIT_FAILURE);       // 프로그램 종료
    }

    // 4. 서버로 HTTP 요청 전송
    send(sock, request, strlen(request), 0);

    // 5. 요청 메시지 출력
    printf("\n--- 요청 ---\n%s", request);

    // 6. 서버로부터 받은 응답 출력
    printf("\n--- 응답 ---\n");

    // 서버로부터 데이터를 읽어서 출력
    while ((bytes_read = read(sock, buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[bytes_read] = '\0';    // 읽은 데이터 뒤에 NULL 문자 추가 (문자열 종료)
        printf("%s", buffer);         // 버퍼 내용 출력
    }

    printf("\n-------------\n");

    // 7. 통신 종료 후 소켓 닫기
    close(sock);
}

int main()
{
    // 서버로 보낼 다양한 HTTP 요청 정의

    const char *get_index =
        "GET /index HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Connection: close\r\n"
        "\r\n"; // /index 페이지 요청 (GET)

    const char *get_root =
        "GET / HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Connection: close\r\n"
        "\r\n"; // 루트(/) 페이지 요청 (GET)

    const char *get_nf =
        "GET /nf HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Connection: close\r\n"
        "\r\n"; // 존재하지 않는 /nf 페이지 요청 (GET)

    const char *get_ise =
        "GET /ise HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Connection: close\r\n"
        "\r\n"; // 에러를 발생시키는 /ise 페이지 요청 (GET)

    const char *post_index =
        "POST /index HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: 9\r\n"
        "Connection: close\r\n"
        "\r\n"
        "data=test"; // /index 페이지에 POST 요청 (데이터 포함)

    const char *post_nf =
        "POST /nf HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: 9\r\n"
        "Connection: close\r\n"
        "\r\n"
        "data=test"; // 존재하지 않는 /nf 페이지에 POST 요청

    const char *put_test =
        "PUT /index HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Content-Length: 0\r\n"
        "Connection: close\r\n"
        "\r\n"; // /index 페이지에 PUT 요청 (테스트용)

    // 각각의 요청을 서버로 보내는 함수 호출
    send_request(get_index);
    send_request(get_root);
    send_request(get_nf);
    send_request(get_ise);
    send_request(post_index);
    send_request(post_nf);
    send_request(put_test);

    return 0; // 프로그램 정상 종료
}
