#include "unp.h"

int Tcp_listen(const char *hostname, const char *service, socklen_t *addrlenp)
{
    int listenfd;
    struct addrinfo hints, *res, *ressave;
    const int on = 1;
    bzero(&hints, sizeof(hints));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int n;
    if ((n = getaddrinfo(hostname, service, &hints, &res)) != 0) {
        err_quit("Tcp_listen error for %s, %s: %s",
                hostname, service, gai_strerror(n));
    }
    ressave = res;
    do {
        listenfd = Socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (listenfd < 0) {
            continue;
        }
        Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0) {
            break;
        }
        Close(listenfd);
    } while ((res = res->ai_next) != NULL);

    if (res == NULL) {
        err_sys("Tcp_listen error for %s, %s", hostname, service);
    }

    Listen(listenfd, LISTENQ);
    if (addrlenp) {
        *addrlenp = res->ai_addrlen;
    }

    freeaddrinfo(ressave);
    return listenfd;
}
