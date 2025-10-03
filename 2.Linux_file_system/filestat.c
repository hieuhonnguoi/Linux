#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

int main(int argc, char *argv[] ){
    struct stat fileStat; 

    
    if (argc != 2){
        fprintf(stderr,"Usage: %s <file_path>\n", argv[0]);
        return 1;
    }

    char *path = argv[1];

    if(lstat(path,&fileStat)<0){      
        perror("lstat");              
        return 1;
    }

    printf("File Path: %s\n", path);

    printf("File Type: ");
    if(S_ISREG(fileStat.st_mode)){
        printf("Regular File\n");
    } else if (S_ISDIR(fileStat.st_mode)){
        printf("Dicectory\n");
    }else{
        printf("Other\n");
    }

    printf("Size: %ld bytes\n",fileStat.st_size);

    char time[100];
    struct tm*tm_info = localtime(&fileStat.st_mtime);
    strftime(time, sizeof(time), "%Y-%m-%d %H:%M:%S",tm_info);
    printf("Last Modified: %s\n", time);
    return 0;
}