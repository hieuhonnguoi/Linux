#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "unix_dgram.sock"

int main() {
    int server_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    struct sockaddr_un addr = {0};
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCK_PATH);

    unlink(SOCK_PATH);
    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));

    printf("Unix Datagram Server waiting...\n");
    char buf[100];
    struct sockaddr_un cli_addr;
    socklen_t len = sizeof(cli_addr);
    recvfrom(server_fd, buf, sizeof(buf), 0, (struct sockaddr*)&cli_addr, &len);

    printf("Server received: %s\n", buf);

    close(server_fd);
    unlink(SOCK_PATH);
    return 0;
}
