#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

int main(int argc, char *argv[] ){
    struct stat fileStat; //khai báo fileStat để lưu kết quả từ lstat()

    //Kiểm tra thông số dòng lệnh. Nếu không dùng đúng sẽ sử dụng stderr
    if (argc != 2){
        fprintf(stderr,"Usage: %s <file_path>\n", argv[0]);
        return 1;
    }

    char *path = argv[1];

    //Lấy dữ liệu
    if(lstat(path,&fileStat)<0){      //đọc metadata cảu đối tượng ở path và lưu vào fileStat
        perror("lstat");              //nếu có lỗi màn hình sẽ hiển thị thông báo kèm lỗi
        return 1;
    }

    //In đường dẫn 
    printf("File Path: %s\n", path);

    //In loại tệp
    printf("File Type: ");
    if(S_ISREG(fileStat.st_mode)){
        printf("Regular File\n");
    } else if (S_ISDIR(fileStat.st_mode)){
        printf("Dicectory\n");
    }else{
        printf("Other\n");
    }

    //In kích thước tệp
    printf("Size: %ld bytes\n",fileStat.st_size);

    char time[100];
    struct tm*tm_info = localtime(&fileStat.st_mtime);
    strftime(time, sizeof(time), "%Y-%m-%d %H:%M:%S",tm_info);
    printf("Last Modified: %s\n", time);
    return 0;
}