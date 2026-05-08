#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    char message[30];
    int option, str_len;
    socklen_t optlen, clnt_adr_sz;
    struct sockaddr_in serv_adr, clnt_adr;

    if (argc != 2){
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    // 소켓 생성
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1){
        printf("socket() error\n");
        exit(1);
    }

    optlen = sizeof(option);
    option = true;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (void*)&option, optlen);

    // 서버 주소 설정
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    // bind
    if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))){
        printf("bind() 오류 발생\n");
    }

    // listen
    if(listen(serv_sock, 5) == -1){
        printf("listen() 오류 발생\n");
        exit(1);
    }

    clnt_adr_sz = sizeof(clnt_adr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);

    while(str_len = read(clnt_sock, message, sizeof(message)) != 0){
        if(str_len == -1){
            printf("read() 오류 발생\n");
            exit(1);
        }
        
        write(clnt_sock, message, str_len);
        write(1, message, str_len);
    }
    
        
    close(clnt_sock);
    close(serv_sock);
    return 0;
}