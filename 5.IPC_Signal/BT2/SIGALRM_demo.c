#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int count = 0;

void alarm_handler(int sig){

    count++;
    printf("Timer: %d\n",count);

    if (count < 10){
        alarm(1);
    }else{
        exit(1);
    }
}

int main(int argc, const char *argv[]){

    signal(SIGALRM,alarm_handler);

    alarm(1);

    while(1){
        pause();
    }

    return 0;
}