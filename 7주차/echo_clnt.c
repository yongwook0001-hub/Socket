#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
    int sock;
    char message[BUF_SIZE];
    int str_len;
    struct sockaddr_in server_addr;

    if (argc != 3){
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    // 소켓 생성
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1){
        printf("socket() error\n");
        exit(1);
    }

    // 서버 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    // 서버 연결
    if(connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        printf("connect() error\n");
        exit(1);
    }

    printf("Connected to server.\n");

    while(1)
    {
        // 사용자 입력
        fputs("Input message(Q to quit): ", stdout);
        fgets(message, BUF_SIZE, stdin);

        // 🔥 서버로 먼저 보냄 (q도 포함)
        write(sock, message, strlen(message));

        // 🔥 서버 응답 받기
        str_len = read(sock, message, BUF_SIZE - 1);
        if (str_len == 0)  // 서버가 연결 끊음
            break;

        message[str_len] = 0;
        printf("Message from server: %s", message);

        // 🔥 q 입력 시 종료
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;
    }

    close(sock);
    return 0;
}