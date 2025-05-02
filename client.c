#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 4096

void send_request(const char *request)
{
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int bytes_read;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0)
    {
        perror("inet_pton failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    send(sock, request, strlen(request), 0);

    printf("\n--- 요청 ---\n%s", request);
    printf("\n--- 응답 ---\n");

    while ((bytes_read = read(sock, buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[bytes_read] = '\0';
        printf("%s", buffer);
    }
    printf("\n-------------\n");

    close(sock);
}

int main()
{
    const char *get_index =
        "GET /index HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Connection: close\r\n"
        "\r\n";

    const char *get_root =
        "GET / HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Connection: close\r\n"
        "\r\n";

    const char *get_nf =
        "GET /nf HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Connection: close\r\n"
        "\r\n";

    const char *get_ise =
        "GET /ise HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Connection: close\r\n"
        "\r\n";

    const char *post_index =
        "POST /index HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: 9\r\n"
        "Connection: close\r\n"
        "\r\n"
        "data=test";

    const char *post_nf =
        "POST /nf HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: 9\r\n"
        "Connection: close\r\n"
        "\r\n"
        "data=test";

    const char *put_test =
        "PUT /index HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Content-Length: 0\r\n"
        "Connection: close\r\n"
        "\r\n";

    send_request(get_index);
    send_request(get_root);
    send_request(get_nf);
    send_request(get_ise);
    send_request(post_index);
    send_request(post_nf);
    send_request(put_test);

    return 0;
}