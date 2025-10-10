#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SERVER_PATH "unix_dgram.sock"
#define CLIENT_PATH "unix_dgram_client.sock"

int main() {
    int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    struct sockaddr_un cli_addr = {0};
    cli_addr.sun_family = AF_UNIX;
    strcpy(cli_addr.sun_path, CLIENT_PATH);

    unlink(CLIENT_PATH);
    bind(sock, (struct sockaddr*)&cli_addr, sizeof(cli_addr));

    struct sockaddr_un srv_addr = {0};
    srv_addr.sun_family = AF_UNIX;
    strcpy(srv_addr.sun_path, SERVER_PATH);

    char *msg = "Hello from client";
    sendto(sock, msg, strlen(msg) + 1, 0, (struct sockaddr*)&srv_addr, sizeof(srv_addr));

    close(sock);
    unlink(CLIENT_PATH);
    return 0;
}
