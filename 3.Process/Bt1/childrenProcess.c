#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char const *argv[]){

    int ret_val;
    int status;

    //Khởi tạo tiến trình con
    ret_val = fork();

    if (ret_val < 0){
        printf("Fail to create children process");
    } 
    else if (ret_val == 0) {
        //Tiến trình con
        printf("Children's ID: PID = %d, My parent's ID: PID = %d\n",getpid(),getppid());
        exit(10);
    } else {
        //Tiến trình cha
        printf("Parent's ID: PID = %d, My children's ID: PID = %d\n",getpid(),ret_val);

        wait(&status);
        
        if (WIFEXITED(status)){
            printf("Children exit with status: %d\n",WEXITSTATUS(status));
        }
        else {
            printf("Children did not exit normally");
        }
    }
    return 0;
}