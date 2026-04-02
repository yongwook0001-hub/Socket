// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <arpa/inet.h>
// #include <sys/socket.h>

// #define BUF_SIZE 4096

// void error_handling(char *message);

// int main(int argc, char *argv[])
// {
//     int serv_sock, clnt_sock;
//     char file_name[100];
//     char buf[BUF_SIZE];
//     int len;

//     struct sockaddr_in serv_adr, clnt_adr;
//     socklen_t clnt_adr_size;

//     if (argc != 2) {
//         printf("Usage : %s <port>\n", argv[0]);
//         exit(1);
//     }

//     serv_sock = socket(PF_INET, SOCK_STREAM, 0);
//     if (serv_sock == -1)
//         error_handling("socket() error");

//     memset(&serv_adr, 0, sizeof(serv_adr));
//     serv_adr.sin_family = AF_INET;
//     serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
//     serv_adr.sin_port = htons(atoi(argv[1]));

//     if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
//         error_handling("bind() error");

//     if (listen(serv_sock, 5) == -1)
//         error_handling("listen() error");

//     clnt_adr_size = sizeof(clnt_adr);
//     clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_size);
//     if (clnt_sock == -1)
//         error_handling("accept() error");

//     // ✅ 파일명 받기
//     recv(clnt_sock, file_name, sizeof(file_name), 0);

//     // ✅ 파일 존재 확인
//     FILE *fp = fopen(file_name, "rb");

//     if (fp == NULL) {
//         send(clnt_sock, "NO", 3, 0);
//         close(clnt_sock);
//         return 0;
//     }
//     else {
//         send(clnt_sock, "OK", 3, 0);
//     }

//     // ✅ 파일 수신
//     FILE *write_fp = fopen(file_name, "wb");

//     while ((len = recv(clnt_sock, buf, BUF_SIZE, 0)) > 0) {
//         fwrite(buf, 1, len, write_fp);
//     }

//     fclose(fp);
//     fclose(write_fp);
//     close(clnt_sock);
//     close(serv_sock);
//     return 0;
// }

// void error_handling(char *message)
// {
//     fputs(message, stderr);
//     fputc('\n', stderr);
//     exit(1);
// }

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
    char *file_name = argv[3];
    char buf[BUF_SIZE];
    int tmpSize, fileSize = 0;

    struct sockaddr_in serv_adr;

    if (argc != 4) {
        printf("Usage : %s <IP> <port> <file>\n", argv[0]);
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

    // ✅ 파일명 전송
    send(sock, file_name, strlen(file_name)+1, 0);

    char tmp[3] = {0};
    recv(sock, tmp, 3, 0);

    if (strcmp(tmp, "OK") != 0) {
        printf("파일 없음, 연결 종료\n");
        close(sock);
        return 0;
    }

    printf("서버 확인 완료, 파일 전송 시작\n");

    fp = fopen(file_name, "rb");
    if (fp == NULL)
        error_handling("file open error");

    while (1) {
        tmpSize = fread(buf, 1, BUF_SIZE, fp);
        if (tmpSize == 0) break;

        send(sock, buf, tmpSize, 0);
        fileSize += tmpSize;
    }

    printf("sends %s\n", file_name);
    printf("file transmit end : %d bytes\n", fileSize);

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