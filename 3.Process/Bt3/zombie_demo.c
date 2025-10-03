#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char const *argv[]){

    int ret_val;
    int status;

    ret_val = fork();

    if (ret_val == 0){
        printf("Children process's ID: PID = %d\n", getpid());
        exit(1);
    } else {
        printf("Parent process's ID: PID = %d\n", getppid());
        while(1);
        wait(&status);
    }
    return 0;
}
