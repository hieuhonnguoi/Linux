#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int count = 0;

void sigint_handler(int sig){
    count++;
    printf("Ctrl C: %d time\n",count);

    if(count == 3){
        printf("Exit program\n");
        exit(1);
    }
}

int main(int argc, char const *argv[]){
    signal(SIGINT,sigint_handler);

    while (1){
        printf("Running.....Press Ctrl C\n");
        sleep(1);
    }

    return 0;
}