#include <stdio.h>
#include <unistd.h>

int gval = 10;
int main(int argc, char *argv[]){
    pid_t pid;
    int lval = 20;
    gval++, lval++;

    pid = fork();
    if(pid==0){
        gval +=2;
        lval +=2;
    }
    else{
        gval -=2;
        lval -=2;
    }

    // if(pid ==0) {
    //     printf("Child proc : [%d, %d]\n", gval, lval);
    //     //printf("자식 프로레스 ID : %d\n", getpid());
    //     sleep(30);
    // }
    // else{
    //     printf("Parent Proc : [%d, %d]\n", gval, lval);
    //     //printf("부모 프로세스 ID : %d\n", getpid());
    //     sleep(30);
    // }

    if (pid == 0) {
        printf("자식 프로세스 실행 중 (PID: %d)\n", getpid());
        sleep(30); // 30초 동안 대기
    } else {
        printf("부모 프로세스 실행 중 (PID: %d, 자식 PID: %d)\n", getpid(), pid);
        sleep(30); // 30초 동안 대기
    }
    return 0;
}