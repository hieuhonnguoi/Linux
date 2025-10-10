#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "unix_stream.sock"

int main() {
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un addr = {0};
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCK_PATH);

    unlink(SOCK_PATH);
    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 1);

    printf("Unix Stream Server listening...\n");
    int client_fd = accept(server_fd, NULL, NULL);

    char buf[100];
    read(client_fd, buf, sizeof(buf));
    printf("Server received: %s\n", buf);

    close(client_fd);
    close(server_fd);
    unlink(SOCK_PATH);
    return 0;
}
