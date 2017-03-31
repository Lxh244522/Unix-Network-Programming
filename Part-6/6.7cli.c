#include "unp.h"

void str_cli(FILE *fp, int sockfd)
{
    // char sendline[MAXLINE], recvline[MAXLINE];
    char buff[MAXLINE];
    int maxfdp1;
    int n;
    fd_set rset;

    FD_ZERO(&rset);
    int stdineof = 0;
    while (1) {
        if (stdineof == 0)
            FD_SET(fileno(fp), &rset);
        FD_SET(sockfd, &rset);
        maxfdp1 = max(fileno(fp), sockfd) + 1;
        Select(maxfdp1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(sockfd, &rset)) {
            if ((n = Read(sockfd, buff, sizeof(buff))) == 0) {
                if (stdineof == 1) {
                    return;
                } else {
                    err_quit("str_cli: server terminated prematurely");
                }
            }
            Writen(fileno(stdout), buff, n);
        }

        if (FD_ISSET(fileno(fp), &rset)) {
            if ((n = Read(fileno(fp), buff, sizeof(buff))) == 0) {
                stdineof = 1;
                Shutdown(sockfd, SHUT_WR);
                FD_CLR(fileno(fp), &rset);
            }
            Writen(sockfd, buff, n);
        }
    }
}

int main(int argc, char const *argv[])
{
    int sockfd;
    struct sockaddr_in servaddr;

    if (argc != 2) {
        err_quit("usage: tcpcli <IPaddress>");
    }

    sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    Connect(sockfd, (SA *)& servaddr, sizeof(servaddr));
    // FILE *fp = fopen("./6.4cli.c", "rb");
    str_cli(stdin, sockfd);
    return 0;
}
