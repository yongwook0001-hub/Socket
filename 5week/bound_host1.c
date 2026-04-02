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
    int str_len;
    char message[BUF_SIZE]; 
    struct sockaddr_in my_adr, your_adr;
    socklen_t adr_sz;
    
    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1) // serv_sock -> sock으로 수정
        error_handling("socket() error");

    memset(&my_adr, 0, sizeof(my_adr));
    my_adr.sin_family = AF_INET; // serv_adr -> my_adr로 수정
    my_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_adr.sin_port = htons(atoi(argv[1]));

    if (bind(sock, (struct sockaddr*)&my_adr, sizeof(my_adr)) == -1)
        error_handling("bind() error");

    printf("UDP Server Started...\n");

    for(int i=0; i<2; i++) {
        sleep(5); // slepp -> sleep으로 수정 (5초 대기하며 데이터가 쌓이길 기다림)
        adr_sz = sizeof(your_adr);
        str_len = recvfrom(sock, message, BUF_SIZE, 0, (struct sockaddr*)&your_adr, &adr_sz);
        
        message[str_len] = 0; // 문자열 끝 처리 추가
        printf("- Message %d (from %s:%d): %s\n", 
                i+1, inet_ntoa(your_adr.sin_addr), ntohs(your_adr.sin_port), message);
    }

    close(sock);
    return 0;
}

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}