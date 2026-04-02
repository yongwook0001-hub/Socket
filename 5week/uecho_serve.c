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
    int serv_sock;
    int str_len;
    char message[BUF_SIZE]; 
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
    
    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    // UDP 소켓 생성
    serv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");

    printf("UDP Server Started. Waiting for messages...\n");

    while(1) {
        clnt_adr_sz = sizeof(clnt_adr);

        // recvfrom으로 데이터 수신 (상대방 주소 정보가 clnt_adr에 채워짐)
        str_len = recvfrom(serv_sock, message, BUF_SIZE, 0, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
        
        printf("\n[Received Data Information]\n");
        printf("- Client IP: %s\n", inet_ntoa(clnt_adr.sin_addr)); // 점진수 IP 추출
        printf("- Client Port: %d\n", ntohs(clnt_adr.sin_port));   // 포트 번호 추출
        printf("- Server Port: %d (My Port)\n", atoi(argv[1]));

        // 에코(Echo): 받은 메시지를 그대로 다시 전송
        sendto(serv_sock, message, str_len, 0, (struct sockaddr*)&clnt_adr, clnt_adr_sz);
        
        message[str_len] = 0; // 문자열 끝 처리
        printf("- Message: %s", message);
    }

    close(serv_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}