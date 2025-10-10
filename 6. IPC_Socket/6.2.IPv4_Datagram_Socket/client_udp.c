#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    char *msg = "Hello";
    sendto(sock, msg, strlen(msg) + 1,0 ,(struct sockaddr*)&addr, sizeof(addr));
    
    close(sock);
    return 0;
}
