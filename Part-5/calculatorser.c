#include "unp.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void echo_rev(int connfd)
{
    ssize_t n = 0;
    char buff[MAXLINE], opera;
    double val1, val2;

    while ((n = Readline(connfd, buff, MAXLINE)) != 0) {
        // printf("%s", buff);
        if (sscanf(buff, "%f%c%f", &val1, &opera, &val2) == 3) {
            // printf("%d %d\n", val1, val2);
            double result = 0.0;
            switch (opera) {
                case '+':
                    result = val1 + val2;
                    break;
                case '-':
                    result = val1 - val2;
                    break;
                case '*':
                    result = val1 * val2;
                    break;
                case '/':
                    result = val1 / val2;
                    break;
                default:
                    result = -1;
                    break;
            }
            snprintf(buff, sizeof(buff), "%d\n", result);
            // printf("%s\n", buff);
        } else {
            // printf("2-\n");
            snprintf(buff, sizeof(buff), "input error\n");
        }
        n = strlen(buff);
        Writen(connfd, buff, n);
    }
    return;
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
