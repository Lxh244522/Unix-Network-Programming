#include "unp.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void echo_rev(int connfd)
{
    ssize_t n = 0;
    char buff[MAXLINE];

again:
    while ((n = read(connfd, buff, MAXLINE)) > 0) {
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
    pid_t pid;
    int stat;

    while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated\n", pid);
    return;
}

int main(int argc, char const *argv[]) {
    /* code */
    int listenfd, connfd;
    struct sockaddr_in servaddr;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);
    Signal(SIGCHLD, sig_child);         /* must call waitpid() */
    while (1) {
        // connfd = Accept(listenfd, (SA *)NULL, NULL);
        if ( (connfd = accept(listenfd, (SA *)NULL, NULL)) < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                err_sys("accept error");
            }
        }

        pid_t pid;
        if ((pid = fork()) == 0) {      /* child precess */
            Close(listenfd);
            echo_rev(connfd);
            Close(connfd);
            exit(0);
        }

        Close(connfd);
    }

    return 0;
}
