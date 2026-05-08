#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

void read_childproc(int sig){
    int status;
    pid_t id = waitpid(-1, &status, WNOHANG);
    if(WIFEXITED(status)){
        printf("Removed proc id : %d\n", id);
        printf("Child send : %d\n", WEXITSTATUS(status));
        }
    
}

int main(int argc, char *argv[]){
    pid_t pid;
    struct sigaction act;
    
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, 0);

    // 첫 번째 자식 생성
    pid = fork();
    if(pid == 0){
        puts("Hi, I am a child process (1)");
        sleep(10);
        return 12;
    }
    else{
        printf("Child Proc ID : %d \n", pid);
        
        // 두 번째 자식 생성
        pid = fork();
        if(pid == 0){
            puts("Hi, I am a child process (2)");
            sleep(10);
            exit(24);
        }
        else{
            int i;
            printf("Child proc id : %d \n", pid);
            for(i=0; i<5; i++){
                puts("wait...");
                sleep(5);
            }
        }
    }

    printf("SIGCHLD의 값: %d\n", SIGCHLD);
    return 0;
}