#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char const *argv[]){
    int ret_val;
    int status;

    //Create enviroment
    setenv("MYCOMMAND","ls",1);

    //Initial 
    ret_val = fork();

    if(ret_val < 0){
        printf("Fail to run children process");
        exit(1);
    } else if(ret_val == 0){
        //Child process
        printf("Children's ID: PID = %d\n", getpid());

        char *cmt = getenv("MYCOMMAND");
        if (cmt == NULL){
            fprintf(stderr,"MY_COMMAND is not set\n");
            exit(1);
        }else{
            printf("Children excuting with: %s\n",cmt);
        }

        execlp(cmt,cmt,NULL);
        perror("Fail to execlp");
        exit(1);

    }else{
        //Parent process
        printf("Parent's ID: PID = %d\n", getpid());
        wait(&status);
        if(WIFEXITED(status)){
            printf("Child exit with status: %d",WEXITSTATUS(status));
        }else{
            printf("Child exit unnormally");
        }
    }
    return 0;
}

