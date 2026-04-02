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
    int sock;
    char message[BUF_SIZE];
    int str_len;
    struct sockaddr_in my_adr, your_adr;
    socklen_t adr_sz;

    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&my_adr, 0, sizeof(my_adr));
    my_adr.sin_family = AF_INET;
    my_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_adr.sin_port = htons(atoi(argv[1]));

    if (bind(sock, (struct sockaddr*)&my_adr, sizeof(my_adr)) == -1)
        error_handling("bind() error");

    printf("Server Waiting...\n");

    while (1)
    {
        // 클라이언트
        adr_sz = sizeof(your_adr);
        // 클라이언트가 보낸 메시지 수신
        str_len = recvfrom(sock, message, BUF_SIZE - 1, 0, (struct sockaddr*)&your_adr, &adr_sz);
        message[str_len] = 0;
        printf("Client: %s", message);

        // [종료 조건] 클라이언트가 q나 Q를 보내면
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
        {
            while(1){
            // bye를 입력하고 전송
            printf("Server Reply: ");
            fgets(message, BUF_SIZE, stdin);
            sendto(sock, message, strlen(message), 0, (struct sockaddr*)&your_adr, adr_sz);
    
            if(!strcmp(message, "bye\n")) {
                    printf("Server closing...\n");
                    close(sock);
                    return 0; // 프로그램 완전히 종료
                } 

            printf("You must type 'bye' to exit.\n");
            }
        }

        // 2. 서버의 메시지 입력 및 전송
        printf("Server Reply: ");
        fgets(message, BUF_SIZE, stdin);
        sendto(sock, message, strlen(message), 0, (struct sockaddr*)&your_adr, adr_sz);
    }
    close(sock);
    return 0;

}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}