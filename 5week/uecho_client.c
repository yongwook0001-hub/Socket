#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char *argv[])
{
    int sock;
    char message[BUF_SIZE];
    int str_len;
    socklen_t adr_sz;
    struct sockaddr_in serv_adr, from_adr;
    
    // 파일 포인터
    FILE *fp;

    if (argc != 3) {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    // message.txt 파일을 쓰기 모드("w")로 생성 및 오픈
    fp = fopen("message.txt", "w");
    if (fp == NULL)
        error_handling("file open error");

    while (1)
    {
        // 1. 메시지 입력 및 전송
        printf("Insert message (q to quit): ");
        fgets(message, sizeof(message), stdin);
        
        sendto(sock, message, strlen(message), 0, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
        
        // 클라이언트가 보낸 메시지 기록
        fputs("Client: ", fp);
        fputs(message, fp);

        // 2. 서버로부터 수신
        adr_sz = sizeof(from_adr);
        str_len = recvfrom(sock, message, BUF_SIZE - 1, 0, (struct sockaddr*)&from_adr, &adr_sz);
        message[str_len] = 0;

        printf("Message from server: %s", message);

        // [파일 저장] 서버가 보낸 메시지 기록
        fputs("Server: ", fp);
        fputs(message, fp);

        // 종료 조건: 서버가 bye를 보내면 종료
        if (!strcmp(message, "bye\n"))
            break;
    }

    // 파일 닫기
    fclose(fp);
    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}