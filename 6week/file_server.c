#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;
    char buf[BUF_SIZE];
    FILE *fp;
    int read_cnt; 

    if (argc != 2){
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        exit(1);

    if(listen(serv_sock, 5) == -1)
        exit(1);

    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    printf("Client connected...\n");

    char clnt_file_name[BUF_SIZE];
    memset(clnt_file_name, 0, sizeof(clnt_file_name));

    // 1. 파일 이름 수신
    int name_len = read(clnt_sock, clnt_file_name, sizeof(clnt_file_name) - 1);
    if(name_len <= 0) {
        close(clnt_sock);
        close(serv_sock);
        return 0;
    }
    clnt_file_name[name_len] = '\0';

    if(strcmp(clnt_file_name, "exit") == 0){
        printf("프로그램 종료 요청 수신\n");
        close(clnt_sock);
        close(serv_sock);
        return 0; 
    }

    printf("클라이언트가 전송한 파일명 : %s\n", clnt_file_name);

    // 2. 파일 생성
    fp = fopen(clnt_file_name, "wb");
    if(fp == NULL) {
        perror("File open error");
        close(clnt_sock);
        return 1;
    }

    // 파일 내부에 파일 이름 기록
    fprintf(fp, "파일명 : %s\n", clnt_file_name);

    // 3. 파일 데이터 수신 및 저장
    while((read_cnt = read(clnt_sock, buf, BUF_SIZE)) != 0){
        fwrite(buf, 1, read_cnt, fp);
    }

    printf("파일 수신 완료.\n");

    // 4. "Thank you" 전송
    write(clnt_sock, "Thank you", 10);

    fclose(fp);
    close(clnt_sock);
    close(serv_sock);
    return 0;
}