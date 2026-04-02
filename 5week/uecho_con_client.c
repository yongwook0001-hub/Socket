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
    char msg1[] = "Hi (from Connected UDP)";
    char msg2[] = "I am focusing only on you";
    char msg3[] = "Nice to meet you";

    struct sockaddr_in serv_adr; 

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

    // (Connected UDP)
    if (connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error");

    printf("Connected UDP Client Socket Created.\n");

    // connect를 했으므로 전송 시 목적지 정보를 생략하는 write/send 사용 가능
    write(sock, msg1, strlen(msg1));
    write(sock, msg2, strlen(msg2));
    write(sock, msg3, strlen(msg3));

    printf("3 Messages sent using write().\n");
    
    close(sock);
    return 0;
}

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}