#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void timeout(int sig){
    if(sig==SIGALRM) puts("Time out!");
    alarm(3);
}

void keycontrol(int sig){
    if(sig == SIGINT) puts("CTRL+C pressed");
}

int main(int argc, char *argv[]){
    int i;
    signal(SIGALRM, timeout);
    signal(SIGINT, keycontrol);
    alarm(3);
    for(i=0 ; i< 5 ; i++){
        puts("wait...");
        sleep(100);
    }

    printf("SIGINT의 값: %d\n", SIGINT);
    printf("SIGALRM의 값: %d\n", SIGALRM);

    return 0;
}

