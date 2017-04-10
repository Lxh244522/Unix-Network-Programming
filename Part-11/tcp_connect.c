#include "unp.h"

int Tcp_connect(const char *hostname, const char *service)
{
    int sockfd;
    struct addrinfo hints, *res, *ressave;

    bzero(&hints, sizeof(hints));
    hints.ai_flags = AI_CANONNAME;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int n;
    if ((n = getaddrinfo(hostname, service, &hints, &res)) != 0) {
        err_quit("Tcp_connect error for %s, %s: %s",
                hostname, service, gai_strerror(n));
    }
    ressave = res;
    do {
        sockfd = Socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd < 0) {
            continue;
        }

        if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0) {
            break;
        }

        Close(sockfd);
    } while((res = res->ai_next) != NULL);

    if (res == NULL) {
        err_sys("Tcp_connect error for %s, %s", hostname, service);
    }

    freeaddrinfo(ressave);
    return sockfd;
}
