#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void timeout(int sig){
    if(sig==SIGALRM) puts("Time out!");
    alarm(2);
}

void keycontrol(int sig){
    if(sig == SIGINT) puts("CTRL+C pressed");
}

int main(int argc, char *argv[]){
    int i;
    struct sigaction act;
    act.sa_handler = timeout;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGALRM, &act, 0);

    // SIGINT 설정 
    struct sigaction act_int;
    act_int.sa_handler = keycontrol;
    sigemptyset(&act_int.sa_mask);
    act_int.sa_flags = 0;
    sigaction(SIGINT, &act_int, 0);

    alarm(2);
    for(i=0 ; i< 3 ; i++){
        puts("wait...");
        sleep(100);
    }

    printf("SIGINT의 값: %d\n", SIGINT);
    printf("SIGALRM의 값: %d\n", SIGALRM);

    return 0;
}

