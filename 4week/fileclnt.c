#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 4096


void error_handling(char *message);

int main(int argc, char *argv[])
{
    int sock;
    FILE *fp; 
    char buf[BUF_SIZE];
    char file_name[100];
    int tmpSize =0, fileSize =0;

    struct sockaddr_in serv_adr;

    if (argc != 3) {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");
   
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error");



    // 핵심만 깔끔하게 정리

while(1){
    printf("파일 이름 입력 (exit 입력시 종료): ");
    scanf("%s", file_name);

    if(strcmp(file_name, "exit") == 0){
        printf("프로그램 종료\n");
        close(sock);
        return 0;
    }

    send(sock, file_name, strlen(file_name)+1, 0);

    char tmp[4];
    int n = recv(sock, tmp, 3, 0);
    if(n <= 0) return 0;
    tmp[n] = '\0';

    if(strcmp(tmp, "OK") != 0){
        printf("파일 없음, 프로그램 종료\n");
        close(sock);
        return 0;
    }

    // 🔥 파일 열기
    FILE *fp = fopen(file_name, "rb");

    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // 🔥 파일 크기 먼저 전송
    send(sock, &file_size, sizeof(file_size), 0);

    printf("파일 전송 시작 (크기: %d bytes)\n", file_size);

    while(1){
        int read_bytes = fread(buf, 1, BUF_SIZE, fp);
        if(read_bytes <= 0) break;

        send(sock, buf, read_bytes, 0);
    }

    fclose(fp);
    printf("전송 완료\n");
}

}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
