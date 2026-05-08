#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <netinet/in.h>

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[]) {
    int sock;
    int state;
    int opt_val;
    socklen_t len;

    // 1. 소켓 생성
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) { 
        error_handling("socket() error");
    }

    // 2. 현재 설정값
    len = sizeof(opt_val);
    state = getsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void*)&opt_val, &len);
    if (state) error_handling("getsockopt() error");

    printf("기본 Nagle 알고리즘 상태: %s \n", (opt_val ? "OFF" : "ON"));


    // 3. 반대로 설정 변경
    opt_val = opt_val ? 0 : 1; 
    state = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void*)&opt_val, sizeof(opt_val));
    if (state) error_handling("setsockopt() error");


    // 4. 변경된 값 출력
    len = sizeof(opt_val);
    state = getsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void*)&opt_val, &len);
    if (state) error_handling("getsockopt() error");

    printf("변경 후 Nagle 알고리즘 상태: %s \n", (opt_val ? "OFF" : "ON"));

    close(sock);
    return 0;
}