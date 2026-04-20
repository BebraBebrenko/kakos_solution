#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <inttypes.h>

void disconnect(int s) { exit(0); }

int main(int argc, char **argv) {

    if (argc != 4) {
        return 1;
    }

    struct sigaction config = {.sa_flags = SA_RESTART, .sa_handler = disconnect};

    sigaction(SIGPIPE, &config, NULL);

    char *host = argv[1];
    char *service = argv[2];
    char *key = argv[3];
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd == -1) {
        return 1;
    }

    struct addrinfo hints = {.ai_family = AF_INET, .ai_socktype = SOCK_STREAM};
    struct addrinfo *result;

    if (getaddrinfo(host, service, &hints, &result) != 0) {
        return 1;
    }

    if (connect(socket_fd, result->ai_addr, result->ai_addrlen) != 0) {
        return 1;
    };
    int64_t num;
    FILE *socket_file = fdopen(socket_fd, "r+");

    fprintf(socket_file, "%s\n", key);
    fflush(socket_file);

    if (fscanf(socket_file, "%" SCNd64, &num) != 1) {
        return 0;
    };
    for (int64_t i = 0; i <= num; i++) {
        fprintf(socket_file, "%" PRId64 " ", i);
    }
    fprintf(socket_file, "\n");

    fflush(socket_file);

    uint64_t long_num;
    if (fscanf(socket_file, "%" SCNu64, &long_num) != 1) {
        return 0;
    };

    printf("%" PRIu64 "\n", long_num);
    return 0;
}
