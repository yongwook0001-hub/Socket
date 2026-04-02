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
    int serv_sock, clnt_sock;
    char file_name[100];
    int len;
    char buf[BUF_SIZE];
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_size;
    

    //
    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }
    //소켓 생성
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");

    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    clnt_adr_size = sizeof(clnt_adr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_size);
        if (clnt_sock == -1)
            error_handling("accept() error");

    while(1){
    int name_len = recv(clnt_sock, file_name, sizeof(file_name), 0);

    if(name_len <= 0){
        printf("클라이언트 종료\n");
        break;
    }

    FILE *fp = fopen(file_name, "rb");

    if(fp == NULL){
        send(clnt_sock, "NO", 2, 0);
        continue;
    }
    else{
        send(clnt_sock, "OK", 2, 0);
        fclose(fp);
    }

    // 🔥 파일 크기 받기
    int file_size;
    recv(clnt_sock, &file_size, sizeof(file_size), 0);

    FILE *write_fp = fopen(file_name, "wb");

    int total = 0;
    while(total < file_size){
        int len = recv(clnt_sock, buf, BUF_SIZE, 0);
        fwrite(buf, 1, len, write_fp);
        total += len;
    }

    fclose(write_fp);
    printf("수신 완료: %s (%d bytes)\n", file_name, file_size);
}

    close(clnt_sock);
    close(serv_sock);
    return 0;
}


void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
