#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char const *argv[]){

    int ret_val;
    int status;

    //Khởi tạo chương trình con
    ret_val = fork();

    if (ret_val == 0){
        //Tiến trình con
        printf("Children process's ID: PID = %d\n", getpid());
        exit(1);
    } else {
        printf("Parent process's ID: PID = %d\n", getppid());
        while(1);
    
        wait(&status);
    }
    return 0;
}

//Tình trạng zombie xảy ra khi tiến trình đã kết thúc mà chương trình cha vẫn chưa gọi ra lệnh wait() làm hệ thống bị treo
//và tiến trình con rơi vào trạng thái zombie. Ý nghĩa của trạng thái này là để tránh tình trạng hệ thống bị treo và giúp tiến trình cha
//vẫn nhận thông tin đầy đủ từ tiến trình con