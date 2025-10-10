#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(12345);

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));

    printf("Sever waiting...\n");
    char buf[100];
    struct sockaddr_in cli_addr;
    socklen_t len = sizeof(cli_addr);
    recvfrom(server_fd, buf, sizeof(buf), 0, (struct sockaddr*)&cli_addr, &len);

    printf("Server received: %s\n", buf);

    close(server_fd);
    return 0;
}