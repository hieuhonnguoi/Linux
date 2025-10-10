#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>

volatile sig_atomic_t sigint_flag;

void sigint_handler(int sig){
    sigint_flag = 1;
}

void sigterm_handler(int sig){
    printf("\nSigterm received. Exiting....\n");
    exit(0);
}

int main(int argc, char const *argv[]){
    signal(SIGINT,sigint_handler);
    signal(SIGTERM,sigterm_handler);

    fd_set readfds;
    char buff[256];
    while(1) {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);

        int ret = select(STDIN_FILENO + 1, &readfds,NULL,NULL,NULL);

        if(ret > 0 && FD_ISSET(STDIN_FILENO, &readfds)) { //have input
            if(fgets(buff, sizeof(buff), stdin) != NULL){
                printf("Input: %s",buff);
            }
        }

        if(sigint_flag){
            printf("Sigint received.\n");
            sigint_flag = 0;
        }
    }
    return 0;
}