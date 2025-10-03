#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main (int argc, char const *argv[]){

    pid_t child_pid;
    int status;

    child_pid = fork();

    if (child_pid == 0){
        for (int i=0;i<5;i++){
            printf("Children's ID: PID = %d\n", getpid());
            printf(" My Parent's ID: PID = %d\n", getppid());
            sleep(2);
        }
    } else {
        printf("Parent's ID: PID = %d\n", getppid());
        exit(0);
    }
    return 0;
}
