#include "unp.h"

void str_echo(int connfd)
{
    char *buff[MAXLINE];
    ssize_t n;
again:
    while ((n = Read(connfd, buff, MAXLINE)) > 0) {
        Writen(connfd, buff, n);
    }
    if (n < 0 && errno == EINTR) {
        goto again;
    } else if (n < 0) {
        err_sys("str_echo: read error");
    }
}

void sig_child(int signo)
{
    if (signo == SIGCHLD) {
        pid_t pid;
        int status;
        while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
            printf("child process %d terminated\n", pid);
        }
        return;
    }
}

int main(int argc, char const *argv[])
{
    int listenfd, connfd, udpfd, nready, maxfd;
    char mesg[MAXLINE];
    pid_t childpid;
    fd_set rset;
    ssize_t n;
    socklen_t len;
    const int on = 1;
    struct sockaddr_in cliaddr, servaddr;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    udpfd = Socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(udpfd, (SA *) &servaddr, sizeof(servaddr));

    Signal(SIGCHLD, sig_child);

    FD_ZERO(&rset);
    maxfd = max(listenfd, udpfd) + 1;

    while (1) {
        FD_SET(listenfd, &rset);
        FD_SET(udpfd, &rset);
        if ((nready = select(maxfd, &rset, NULL, NULL, NULL)) < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                err_sys("select error");
            }
        }

        if (FD_ISSET(listenfd, &rset)) {
            len = sizeof(cliaddr);
            connfd = Accept(listenfd, (SA *) &cliaddr, &len);

            if ((childpid = Fork()) == 0) {
                Close(listenfd);
                str_echo(connfd);
                exit(0);
            }
            Close(connfd);
        }

        if (FD_ISSET(udpfd, &rset)) {
            len = sizeof(cliaddr);
            n = Recvfrom(udpfd, mesg, MAXLINE, 0, (SA *) &cliaddr, &len);

            Sendto(udpfd, mesg, n, 0, (SA *) &cliaddr, len);
        }
    }
    return 0;
}
