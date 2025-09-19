#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char const *argv[]){
    int ret_val;
    int status;

    //Thiết lập môi trường
    setenv("MYCOMMAND","ls",1);

    //Khởi tạo chương trình con
    ret_val = fork();

    if(ret_val < 0){
        printf("Fail to run children process");
        exit(1);
    } 
    else if(ret_val == 0){
        //Chạy chương trình con
        printf("Children's ID: PID = %d\n", getpid());

        char *cmt = getenv("MYCOMMAND");
        if (cmt == NULL){
            fprintf(stderr,"MY_COMMAND is not set\n");
            exit(1);
        }else{
            printf("Children excuting with: %s\n",cmt);
        }

        //thay thế bằng lệnh exac
        execlp(cmt,cmt,NULL);
        //nếu fail
        perror("Fail to execlp");
        exit(1);
    }else{
        //Chạy chương trình cha
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

//Sau khi thực thi lệnh exec() thì địa chỉ của tiến trình con vẫn giữ nguyên 
// tuy nhiên mã nguồn không còn như lúc đầu mà chuyển sang không gian mới
// chương trình sẽ không tiếp tục các lệnh sau exac 