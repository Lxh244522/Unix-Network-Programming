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
        int status;
        pid_t pid;
        while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
            printf("child process %d terminated\n", pid);
        }
        return;
    }
}

int main(int argc, char const *argv[])
{
    int listenfd, connfd;
    struct sockaddr_in servaddr;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(listenfd, (SA *)& servaddr, sizeof(servaddr));
    Listen(listenfd, LISTENQ);

    Signal(SIGCHLD, sig_child);

    int client[FD_SETSIZE];
    int maxfd, maxi, nready, i, sockfd;;
    fd_set rset, allset;

    maxfd = listenfd;
    maxi = -1;
    for (int i=0; i<FD_SETSIZE; i++)
        client[i] = -1;

    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);
    while (1) {
        rset = allset;
        sagain:
        if ((nready = select(maxfd + 1, &rset, NULL, NULL, NULL)) < 0) {
            if (nready < 0 && errno == EINTR) {
                goto sagain;
            } else {
                err_sys("main: select error");
            }
        }

        if (FD_ISSET(listenfd, &rset)) {
            connfd = Accept(listenfd, (SA *)NULL, NULL);

            for (i=0; i<FD_SETSIZE; i++) {
                if (client[i] < 0) {
                    client[i] = connfd;
                    break;
                }
            }
            if (i == FD_SETSIZE)
                err_quit("too many clients");

            FD_SET(connfd, &allset);
            if (connfd > maxfd);
                maxfd = connfd;
            if (i > maxi)
                maxi = i;

            if (--nready <= 0)
                continue;
        }

        for (i=0; i<=maxi; i++) {
            if ((sockfd = client[i]) < 0)
                continue;
            if (FD_ISSET(sockfd, &rset)) {
                pid_t pid;
                if ((pid = fork()) == 0) {
                    Close(listenfd);
                    str_echo(sockfd);
                    Close(sockfd);
                    exit(0);
                }
                Close(sockfd);
                FD_CLR(sockfd, &allset);
                client[i] = -1;
                if (--nready <= 0)
                    break;
            }
        }
        // pid_t pid;
        // if ((pid = fork()) == 0) {
        //     Close(listenfd);
        //     str_echo(connfd);
        //     Close(connfd);
        // }
        // Close(connfd);
    }

    return 0;
}
