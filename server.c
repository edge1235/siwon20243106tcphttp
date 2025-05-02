#include <stdio.h>      // 표준 입출력 함수 사용 (printf, perror 등)
#include <stdlib.h>     // 일반 유틸리티 함수 사용 (exit 등)
#include <string.h>     // 문자열 처리 함수 사용 (strcmp, strlen 등)
#include <unistd.h>     // 유닉스 시스템 호출 함수 사용 (close, read, write 등)
#include <arpa/inet.h>  // 소켓 프로그래밍 함수 사용 (socket, bind, listen, accept 등)

#define PORT 8080       // 서버가 열 포트 번호를 8080으로 설정

// 클라이언트에게 HTTP 응답을 보내는 함수
void send_response(int client_fd, int code, const char *status, const char *body)
{
    char response[4096];  // 응답 메시지를 저장할 버퍼 선언

    // snprintf를 이용해 HTTP 응답 포맷(헤더 + 본문)을 버퍼에 작성
    snprintf(response, sizeof(response),
             "HTTP/1.1 %d %s\r\n"             // 상태줄 (HTTP 버전, 상태 코드, 상태 메시지)
             "Content-Type: text/html\r\n"    // 응답 본문 타입 지정 (HTML 문서)
             "Content-Length: %lu\r\n"         // 본문 크기 지정
             "Connection: close\r\n"           // 응답 후 연결 종료 명시
             "\r\n"                            // 헤더 끝
             "%s",                             // 본문 (HTML)
             code, status, strlen(body), body);

    // 조립된 응답 메시지를 클라이언트 소켓에 전송
    write(client_fd, response, strlen(response));
}

int main()
{
    int server_fd, client_fd;                 // 서버 소켓과 클라이언트 소켓 디스크립터
    struct sockaddr_in addr;                  // 서버 주소 정보를 담을 구조체
    socklen_t addrlen = sizeof(addr);          // 구조체 크기 저장
    char buffer[4096];                         // 클라이언트 요청을 저장할 버퍼

    // 1. 서버 소켓 생성 (IPv4, TCP 스트림 소켓)
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)  // 소켓 생성 실패 시
    {
        perror("socket failed");  // 에러 메시지 출력
        exit(EXIT_FAILURE);       // 프로그램 종료
    }

    // 2. 소켓 옵션 설정 (SO_REUSEADDR: 포트 재사용 허용)
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 3. 서버 주소 설정
    addr.sin_family = AF_INET;           // IPv4 주소 체계
    addr.sin_addr.s_addr = INADDR_ANY;   // 모든 네트워크 인터페이스로부터 접속 허용
    addr.sin_port = htons(PORT);         // 포트 번호를 네트워크 바이트 오더로 변환하여 저장

    // 4. 소켓과 주소 바인딩
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind failed");   // 바인딩 실패 시 에러 출력
        close(server_fd);        // 소켓 닫기
        exit(EXIT_FAILURE);      // 프로그램 종료
    }

    // 5. 클라이언트 접속 대기 상태로 변경 (백로그 큐 크기 10)
    if (listen(server_fd, 10) < 0)
    {
        perror("listen failed"); // 리슨 실패 시 에러 출력
        close(server_fd);         // 소켓 닫기
        exit(EXIT_FAILURE);       // 프로그램 종료
    }

    printf("Listening on port %d...\n", PORT);  // 서버가 준비되었음을 출력

    // 6. 메인 루프: 클라이언트 요청 처리 반복
    while (1)
    {
        // 클라이언트 접속 수락
        client_fd = accept(server_fd, (struct sockaddr *)&addr, &addrlen);
        if (client_fd < 0)
        {
            perror("accept failed"); // 접속 실패 시 에러 출력
            continue;                // 다음 연결 대기
        }

        // 클라이언트 요청 읽기
        int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) // 읽은 데이터가 있을 경우
        {
            buffer[bytes_read] = '\0';  // 문자열 끝 표시
            printf("Received request:\n%s\n", buffer);  // 요청 출력

            // 요청에서 HTTP 메서드와 경로 추출
            char method[8], path[1024];
            sscanf(buffer, "%s %s", method, path); // 예: "GET /index"

            // 요청 처리
            if (strcmp(method, "GET") == 0) // GET 요청 처리
            {
                if (strcmp(path, "/index") == 0 || strcmp(path, "/") == 0)
                {
                    // /index 또는 / 요청인 경우 → 200 OK
                    send_response(client_fd, 200, "OK",
                    "<html><body><h1>200 OK (GET)</h1><p>Hello siwon</p></body></html>");
                }
                else if (strcmp(path, "/nf") == 0)
                {
                    // /nf 요청인 경우 → 404 Not Found
                    send_response(client_fd, 404, "Not Found",
                    "<html><body><h1>404 Not Found (GET)</h1><p>Page not found.</p></body></html>");
                }
                else if (strcmp(path, "/ise") == 0)
                {
                    // /ise 요청인 경우 → 500 Internal Server Error
                    send_response(client_fd, 500, "Internal Server Error",
                    "<html><body><h1>500 Internal Server Error (GET)</h1><p>Server error occurred.</p></body></html>");
                }
            }
            else if (strcmp(method, "POST") == 0) // POST 요청 처리
            {
                if (strcmp(path, "/index") == 0 || strcmp(path, "/") == 0)
                {
                    // /index 또는 / 요청에 POST → 200 OK
                    send_response(client_fd, 200, "OK",
                    "<html><body><h1>200 OK (POST)</h1><p>POST request successful</p></body></html>");
                }
                else if (strcmp(path, "/nf") == 0)
                {
                    // /nf 요청에 POST → 404 Not Found
                    send_response(client_fd, 404, "Not Found",
                    "<html><body><h1>404 Not Found (POST)</h1><p>Page not found.</p></body></html>");
                }
            }
            else
            {
                // GET, POST 이외의 메서드는 → 400 Bad Request
                send_response(client_fd, 400, "Bad Request",
                "<html><body><h1>400 Bad Request</h1><p>Unsupported method.</p></body></html>");
            }
        }

        // 요청 처리 후 클라이언트 소켓 닫기
        close(client_fd);
    }

    // 서버 소켓 닫기 (무한 루프 때문에 실제로는 여기 도달하지 않음)
    close(server_fd);
    return 0;
}