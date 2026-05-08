#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling (char *message);

int main(int argc, char *argv[]){
    int recv_sock;
    int str_len;
    char buf[BUF_SIZE];
    struct sockaddr_in adr;
    
    // 사용자가 포트 번호 1개만 넘겨주는 것이 맞으므로 실행 인자 개수는 2개 검사
    if(argc != 2){
        printf("Usage : %s <PORT>\n", argv[0]);
        exit(1);
    }

    recv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&adr, 0, sizeof(adr));
    adr.sin_family = AF_INET;
    adr.sin_addr.s_addr = htonl(INADDR_ANY); // 브로드캐스트 패킷 수신을 위해 INADDR_ANY 유지
    adr.sin_port = htons(atoi(argv[1]));     // 첫 번째 인자를 포트로 사용

    if(bind(recv_sock, (struct sockaddr *)&adr, sizeof(adr)) == -1){
        error_handling("bind() error");
    }
    
    printf("Ready to receive Broadcast messages on port %s...\n", argv[1]);

    while(1){
        // 수신 대기 (센더가 전송하면 가로채어 출력)
        str_len = recvfrom(recv_sock, buf, BUF_SIZE-1, 0, NULL, 0);
        if(str_len < 0){
            break;
        }
        buf[str_len] = 0;
        
        // 데이터가 들어오는 즉시 터미널 화면에 밀어내기 출력
        fputs(buf, stdout);
        fflush(stdout); 
    }
    close(recv_sock);
    return 0;
}

void error_handling(char *message){
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}