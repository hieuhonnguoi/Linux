#define _GNU_SOURCE
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_MSG 100
#define MAX_LINE 512

typedef struct {
    int id;                     // unique connection ID
    int sock;                   // socket descriptor
    char ip[64];                // peer IP
    int port;                   // peer port
    pthread_t rx_thread;        // receiving thread
    atomic_int alive;           // alive flag
} peer_t;

typedef struct {
    peer_t *items;
    size_t size;
    size_t cap;
    pthread_mutex_t mu;
} peer_list_t;

static peer_list_t g_peers = {NULL, 0, 0, PTHREAD_MUTEX_INITIALIZER};
static int g_listen_sock = -1;
static int g_listen_port = 0;
static pthread_t g_accept_thread;
static atomic_int g_running = 1;
static atomic_int g_next_id = 1;

/* -------------------- Utility helpers -------------------- */
static void die(const char* msg) {
    perror(msg);
    exit(1);
}

static void safe_close(int *fd) {
    if (*fd >= 0) { close(*fd); *fd = -1; }
}

/* -------------------- Peer list management -------------------- */
static void peers_init(peer_list_t *pl) {
    pl->cap = 8;
    pl->size = 0;
    pl->items = calloc(pl->cap, sizeof(peer_t));
    if (!pl->items) die("calloc peers");
    pthread_mutex_init(&pl->mu, NULL);
}

static void peers_grow(peer_list_t *pl) {
    if (pl->size < pl->cap) return;
    pl->cap *= 2;
    peer_t *n = realloc(pl->items, pl->cap * sizeof(peer_t));
    if (!n) die("realloc peers");
    pl->items = n;
}

static int peers_add(peer_list_t *pl, int sock, const char *ip, int port, pthread_t rx_th) {
    pthread_mutex_lock(&pl->mu);
    peers_grow(pl);
    peer_t *p = &pl->items[pl->size++];
    p->id = atomic_fetch_add(&g_next_id, 1);
    p->sock = sock;
    snprintf(p->ip, sizeof(p->ip), "%s", ip);
    p->port = port;
    p->rx_thread = rx_th;
    atomic_store(&p->alive, 1);
    int id = p->id;
    pthread_mutex_unlock(&pl->mu);
    return id;
}

static peer_t* peers_find_by_id(peer_list_t *pl, int id) {
    for (size_t i = 0; i < pl->size; ++i) {
        if (pl->items[i].id == id) return &pl->items[i];
    }
    return NULL;
}

static void peers_remove_by_index(peer_list_t *pl, size_t idx) {
    if (idx >= pl->size) return;
    if (pl->items[idx].sock >= 0) close(pl->items[idx].sock);
    pl->items[idx] = pl->items[pl->size - 1];
    pl->size--;
}

static int peers_remove_by_id(peer_list_t *pl, int id) {
    pthread_mutex_lock(&pl->mu);
    for (size_t i = 0; i < pl->size; ++i) {
        if (pl->items[i].id == id) {
            peers_remove_by_index(pl, i);
            pthread_mutex_unlock(&pl->mu);
            return 0;
        }
    }
    pthread_mutex_unlock(&pl->mu);
    return -1;
}

static void peers_broadcast_bye(peer_list_t *pl) {
    pthread_mutex_lock(&pl->mu);
    const char *bye = "BYE";
    for (size_t i = 0; i < pl->size; ++i) {
        if (pl->items[i].sock >= 0) {
            send(pl->items[i].sock, bye, strlen(bye), 0);
            shutdown(pl->items[i].sock, SHUT_RDWR);
        }
    }
    pthread_mutex_unlock(&pl->mu);
}

static void peers_list(peer_list_t *pl) {
    pthread_mutex_lock(&pl->mu);
    if (pl->size == 0) {
        printf("[list] No active connections.\n");
    } else {
        printf("id\tIP\t\tport\n");
        for (size_t i = 0; i < pl->size; ++i) {
            peer_t *p = &pl->items[i];
            printf("%d\t%s\t%d\n", p->id, p->ip, p->port);
        }
    }
    pthread_mutex_unlock(&pl->mu);
}

/* -------------------- Command helpers -------------------- */
static int is_self(const char *ip, int port) {
    if (port != g_listen_port) return 0;
    if (strcmp(ip, "127.0.0.1") == 0) return 1;

    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) return 0;
    struct sockaddr_in tmp = {0};
    tmp.sin_family = AF_INET;
    tmp.sin_port = htons(53);
    inet_pton(AF_INET, "8.8.8.8", &tmp.sin_addr);
    connect(s, (struct sockaddr*)&tmp, sizeof(tmp));
    struct sockaddr_in local = {0};
    socklen_t len = sizeof(local);
    getsockname(s, (struct sockaddr*)&local, &len);
    char local_ip[64];
    inet_ntop(AF_INET, &local.sin_addr, local_ip, sizeof(local_ip));
    close(s);
    return (strcmp(ip, local_ip) == 0);
}

static void print_help(void) {
    printf(
        "Commands:\n"
        "  help                          : Show this help.\n"
        "  myip                          : Print local IP.\n"
        "  myport                        : Print listening port.\n"
        "  connect <dest_ip> <port>      : Connect to a peer.\n"
        "  list                          : List active connections.\n"
        "  send <id> <message>           : Send message (<= 100 chars) to connection id.\n"
        "  terminate <id>                : Close a specific connection.\n"
        "  exit                          : Notify peers, close all, and quit.\n"
    );
}

static void print_myip(void) {
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) { perror("socket"); return; }
    struct sockaddr_in tmp = {0};
    tmp.sin_family = AF_INET;
    tmp.sin_port = htons(53);
    inet_pton(AF_INET, "8.8.8.8", &tmp.sin_addr);
    if (connect(s, (struct sockaddr*)&tmp, sizeof(tmp)) < 0) {
        perror("connect probe"); close(s); return;
    }
    struct sockaddr_in local = {0};
    socklen_t len = sizeof(local);
    if (getsockname(s, (struct sockaddr*)&local, &len) == 0) {
        char ip[64];
        inet_ntop(AF_INET, &local.sin_addr, ip, sizeof(ip));
        printf("%s\n", ip);
    }
    close(s);
}

/* -------------------- Receiver thread -------------------- */
typedef struct {
    int id;
    int sock;
    char ip[64];
    int port;
} rx_arg_t;

static void* rx_thread_main(void *arg_) {
    rx_arg_t *arg = (rx_arg_t*)arg_;
    char buf[1024];

    while (g_running) {
        ssize_t n = recv(arg->sock, buf, sizeof(buf)-1, 0);
        if (n <= 0) break;
        buf[n] = '\0';
        if (strncmp(buf, "BYE", 3) == 0) {
            printf("[peer %d %s:%d] closed.\n", arg->id, arg->ip, arg->port);
            break;
        }
        printf("[msg from %s:%d] %s\n", arg->ip, arg->port, buf);
        fflush(stdout);
    }

    peers_remove_by_id(&g_peers, arg->id);
    close(arg->sock);
    free(arg);
    return NULL;
}

/* -------------------- Accept thread -------------------- */
static void* accept_thread_main(void *unused) {
    (void)unused;
    while (g_running) {
        struct sockaddr_in cli; socklen_t cl = sizeof(cli);
        int cfd = accept(g_listen_sock, (struct sockaddr*)&cli, &cl);
        if (cfd < 0) {
            if (errno == EINTR) continue;
            if (!g_running) break;
            perror("accept"); continue;
        }
        char ip[64];
        inet_ntop(AF_INET, &cli.sin_addr, ip, sizeof(ip));
        int port = ntohs(cli.sin_port);

        rx_arg_t *a = malloc(sizeof(*a));
        a->sock = cfd;
        snprintf(a->ip, sizeof(a->ip), "%s", ip);
        a->port = port;

        pthread_t th;
        pthread_create(&th, NULL, rx_thread_main, a);
        pthread_detach(th);

        int id = peers_add(&g_peers, cfd, ip, port, th);
        a->id = id;

        printf("[accept] new peer id=%d from %s:%d\n", id, ip, port);
    }
    return NULL;
}

/* -------------------- Connect to peer -------------------- */
static int connect_peer(const char *ip, int port) {
    if (port <= 0 || port > 65535) { printf("Invalid port\n"); return -1; }
    if (is_self(ip, port)) {
        printf("Cannot connect to self.\n");
        return -1;
    }
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) { perror("socket"); return -1; }
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &addr.sin_addr) != 1) {
        printf("Invalid IP address\n"); close(s); return -1;
    }
    if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect"); close(s); return -1;
    }

    rx_arg_t *a = malloc(sizeof(*a));
    a->sock = s;
    snprintf(a->ip, sizeof(a->ip), "%s", ip);
    a->port = port;
    pthread_t th;
    pthread_create(&th, NULL, rx_thread_main, a);
    pthread_detach(th);

    int id = peers_add(&g_peers, s, ip, port, th);
    a->id = id;
    printf("[connect] connected to %s:%d as id=%d\n", ip, port, id);
    return id;
}

/* -------------------- Send & terminate -------------------- */
static void send_msg(int id, const char *msg) {
    if ((int)strlen(msg) > MAX_MSG) {
        printf("Message too long (> %d)\n", MAX_MSG);
        return;
    }
    pthread_mutex_lock(&g_peers.mu);
    peer_t *p = peers_find_by_id(&g_peers, id);
    if (!p) { pthread_mutex_unlock(&g_peers.mu); printf("No such id\n"); return; }
    int sock = p->sock;
    char ip[64]; snprintf(ip, sizeof(ip), "%s", p->ip);
    int port = p->port;
    pthread_mutex_unlock(&g_peers.mu);

    send(sock, msg, strlen(msg), 0);
    printf("[sent to %s:%d] %s\n", ip, port, msg);
}

static void terminate_id(int id) {
    pthread_mutex_lock(&g_peers.mu);
    peer_t *p = peers_find_by_id(&g_peers, id);
    if (!p) { pthread_mutex_unlock(&g_peers.mu); printf("No such id\n"); return; }
    int s = p->sock;
    pthread_mutex_unlock(&g_peers.mu);

    const char *bye = "BYE";
    send(s, bye, strlen(bye), 0);
    shutdown(s, SHUT_RDWR);
    printf("[terminate] id=%d closed.\n", id);
}

/* -------------------- Listener -------------------- */
static int start_listener(int port) {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) die("socket");
    int yes = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) die("bind");
    if (listen(s, 16) < 0) die("listen");
    return s;
}

/* -------------------- Main -------------------- */
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <listen_port>\n", argv[0]);
        return 1;
    }
    g_listen_port = atoi(argv[1]);
    if (g_listen_port <= 0 || g_listen_port > 65535) die("invalid port");

    peers_init(&g_peers);
    g_listen_sock = start_listener(g_listen_port);

    pthread_create(&g_accept_thread, NULL, accept_thread_main, NULL);

    printf("P2P chat running. Type 'help' for commands.\n");

    char line[MAX_LINE];
    while (g_running && fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (line[0] == '\0') continue;

        if (strcmp(line, "help") == 0) {
            print_help();
        } else if (strcmp(line, "myip") == 0) {
            print_myip();
        } else if (strcmp(line, "myport") == 0) {
            printf("%d\n", g_listen_port);
        } else if (strncmp(line, "connect ", 8) == 0) {
            char ip[128]; int port;
            if (sscanf(line + 8, "%127s %d", ip, &port) == 2) {
                connect_peer(ip, port);
            } else {
                printf("Usage: connect <ip> <port>\n");
            }
        } else if (strcmp(line, "list") == 0) {
            peers_list(&g_peers);
        } else if (strncmp(line, "send ", 5) == 0) {
            int id; char msg[MAX_LINE];
            if (sscanf(line + 5, "%d %[^\n]", &id, msg) == 2) {
                send_msg(id, msg);
            } else {
                printf("Usage: send <id> <message>\n");
            }
        } else if (strncmp(line, "terminate ", 10) == 0) {
            int id;
            if (sscanf(line + 10, "%d", &id) == 1) {
                terminate_id(id);
            } else {
                printf("Usage: terminate <id>\n");
            }
        } else if (strcmp(line, "exit") == 0) {
            g_running = 0;
            peers_broadcast_bye(&g_peers);
            break;
        } else {
            printf("Unknown command. Type 'help'.\n");
        }
    }

    safe_close(&g_listen_sock);
    pthread_cancel(g_accept_thread);
    pthread_join(g_accept_thread, NULL);

    pthread_mutex_lock(&g_peers.mu);
    for (size_t i = 0; i < g_peers.size; ++i) {
        if (g_peers.items[i].sock >= 0) {
            shutdown(g_peers.items[i].sock, SHUT_RDWR);
            close(g_peers.items[i].sock);
        }
    }
    free(g_peers.items);
    pthread_mutex_unlock(&g_peers.mu);

    printf("Bye.\n");
    return 0;
}
