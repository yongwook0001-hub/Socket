#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define BUF_SIZE 30
void error_handling (char *message);
void read_childproc(int sig);

int main(int argc, char *argv[]){
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;

    pid_t pid;
    struct sigaction act;
    socklen_t adr_sz;
    int str_len, state;
    char buf[BUF_SIZE];

    if(argc !=2){
        printf("Usage : %s <PORT>\n", argv[0]);
        exit(1);
    }

    /* [1단계: 좀비 프로세스 방지 설정] */
    act.sa_handler = read_childproc;     // 자식 프로세스 종료 시 read_childproc 함수 호출
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    state = sigaction(SIGCHLD, &act, 0); // SIGCHLD(자식 종료) 신호 등록

    /* [2단계: 서버 리스닝 소켓 생성 및 설정] */
    serv_sock = socket(PF_INET, SOCK_STREAM, 0); // 문지기 소켓 생성
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    // 주소 할당(bind) 및 연결 대기(listen)
    if(bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");
    if(listen(serv_sock, 5) == -1)
        error_handling("listen() error");
    
    /* [3단계: 클라이언트 접속 수용 루프] */
    while(1){
        adr_sz = sizeof(clnt_adr);
        // 클라이언트 접속 대기 (연결되면 새로운 clnt_sock 생성)
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &adr_sz);
        
        if(clnt_sock == -1) continue;
        else puts("new client connected...");

        // 현재 소켓 번호 출력 (실험 포인트!)
        printf("Parent - serv_sock: %d, clnt_sock: %d\n", serv_sock, clnt_sock);

        /* [4단계: 프로세스 복제(fork)] */
        pid = fork(); 
        
        if(pid == -1){ // 복제 실패 시
            close(clnt_sock);
            continue;
        }

        if(pid == 0){ 
            /* --- [자식 프로세스 영역] --- */
            // 1. 자식은 문지기(serv_sock)가 필요 없으므로 닫음
            close(serv_sock); 

            // 2. 클라이언트와 실제 데이터 주고받기(Echo 서비스)
            while((str_len = read(clnt_sock, buf, BUF_SIZE)) != 0)
            {
                write(clnt_sock, buf, str_len); // 읽은 내용을 그대로 다시 보냄
            }

            // 3. 서비스 종료 후 본인의 클라이언트 소켓 닫기
            close(clnt_sock);
            puts("client disconnected...");
            return 0; // 자식 프로세스 종료 (이때 부모에게 SIGCHLD 신호가 감)
        }
        
        /* --- [부모 프로세스 영역] --- */
        // 원래는 여기서 close(clnt_sock)을 해서 소켓 소유권을 자식에게 넘겨야 함.
        // 현재는 주석 처리되어 있어 부모도 계속 소켓 번호를 가짐 -> 번호가 증가하는 원인.
        else close(clnt_sock);
    }
    close(serv_sock);
    return 0;
}

/* 좀비 프로세스 방지를 위한 핸들러 함수 */
void read_childproc(int sig){
    pid_t pid;
    int status;
    // 종료된 자식의 정보를 읽어와서 메모리에서 완전히 소멸시킴
    pid = waitpid(-1, &status, WNOHANG);
    printf("removed proc id: %d\n", pid);
}

void error_handling(char *message){
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}