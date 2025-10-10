#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

void children_handler(int sig){
    if (sig == SIGUSR1){
        printf("Receiving signal from parent....\n");
    }
}

int main(int argc, char const *argv[]){
    pid_t pid = fork();

    if(pid < 0){
        perror("Fail to create child process");
        exit(1);
    }

    if(pid == 0){ //Children's process
        signal(SIGUSR1,children_handler);
        while(1) {
            pause();
        }
    } else { //Parent's process
        for (int i = 0; i < 5; i++){
            sleep(2);
            kill(pid,SIGUSR1);
        }
    }
    printf("Program finish\n");
    return 0;
}