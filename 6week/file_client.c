#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in server_addr;
    FILE *fp;
    char buf[BUF_SIZE];
    int read_cnt;
    long total_bytes = 0; // 바이트 수 확인용
    char *thank_you = "Thank you";

    if (argc != 3){
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

    printf("업로드할 파일 이름을 입력하시오 : ");
    char filename[BUF_SIZE];
    scanf("%s", filename);

    fp = fopen(filename, "rb");
    if(fp == NULL){
        printf("File Not found\n");
        close(sock);
        return 0;
    }
    // 파일에 파일 이름 전송
    write(sock, filename, strlen(filename));

    usleep(100000); // 잠시 대기
    while((read_cnt = fread(buf, 1, BUF_SIZE, fp)) > 0){
    write(sock, buf, read_cnt); // 내용 전송
    }

    shutdown(sock, SHUT_WR); // 전송 완료 후 소켓의 쓰기 부분 종료

    memset(buf, 0, sizeof(buf));// 서버로부터 "Thank you" 메시지 수신
    read(sock, buf, BUF_SIZE);
    printf("Server says: %s\n", buf);

    fclose(fp);
    close(sock);
    return 0;
}