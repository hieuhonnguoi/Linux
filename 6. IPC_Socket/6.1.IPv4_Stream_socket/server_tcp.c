#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

int main(int argc, char const *argv[]){
    int sever_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(12345);

    bind(sever_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(sever_fd, 1);

    printf("TCP Sever Listening...\n");
    int client_fd = accept(sever_fd, NULL, NULL);

    char buf[100];
    read(client_fd, buf, sizeof(buf));
    printf("Sever received: %s\n", buf);

    close(client_fd);
    close(sever_fd);

    return 0;
}
