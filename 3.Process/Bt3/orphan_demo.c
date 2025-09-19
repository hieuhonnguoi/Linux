#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main (int argc, char const *argv[]){

    pid_t child_pid;
    int status;

    //Khởi tạo chương trình con
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

//Tình trạng orphan xuất hiện khi tiến trình cha kết thúc mà tiến trình con vẫn chưa chạy xong, làm cho tiến trình con
//không còn tiến trình cha nữa. Khi đó hệ thống sẽ mặc định tiến có PID = 1 làm cha của tiến trình con đang chạy khi code trên 
//ban đầu PID của tiến trình cha vẫn hiện chính xác nhưng các lần sau sẽ hiện là 1