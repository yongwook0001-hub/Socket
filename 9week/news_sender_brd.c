#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling (char *message);

int main(int argc, char *argv[]){
    int send_sock;
    struct sockaddr_in broad_adr;
    FILE *fp;
    int so_brd=1;
    char buf[BUF_SIZE];
    if(argc != 3){
        printf("Usage : %s <GroupIP> <PORT>\n", argv[0]);
        exit(1);
    }

    send_sock = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&broad_adr, 0, sizeof(broad_adr));
    broad_adr.sin_family = AF_INET;
    broad_adr.sin_addr.s_addr = inet_addr(argv[1]); //Broadcast IP
    broad_adr.sin_port = htons(atoi(argv[2])); //Broadcast Port

    setsockopt(send_sock, SOL_SOCKET, SO_BROADCAST, (void *)&so_brd, sizeof(so_brd));

    if((fp = fopen("news.txt", "r")) == NULL){
        error_handling("fopen() error");
    }

    while(!feof(fp)){
        if(fgets(buf, BUF_SIZE, fp) != NULL) {
            sendto(send_sock, buf, strlen(buf), 0, (struct sockaddr *)&broad_adr, sizeof(broad_adr));
            printf("Sent: %s", buf);
            sleep(2);
        }
    }

    printf("\nBroadcasting finished successfully.\n");
    fclose(fp);
    close(send_sock);
    return 0;
}

void error_handling(char *message){
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}