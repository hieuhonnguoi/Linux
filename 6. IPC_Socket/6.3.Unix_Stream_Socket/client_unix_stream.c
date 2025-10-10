#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "unix_stream.sock"

int main() {
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un addr = {0};
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCK_PATH);

    connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    char *msg = "Hello from client";
    write(sock, msg, strlen(msg) + 1);

    close(sock);
    return 0;
}
