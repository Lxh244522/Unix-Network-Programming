#include "unp.h"

void str_cli(FILE *fp, int sockfd)
{
    char sendline[MAXLINE], recvline[MAXLINE];
    int maxfdp1;
    fd_set rset;

    FD_ZERO(&rset);
    while (1) {
        FD_SET(fileno(fp), &rset);
        FD_SET(sockfd, &rset);
        maxfdp1 = max(fileno(fp), sockfd) + 1;
        Select(maxfdp1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(sockfd, &rset)) {
            if (Readline(sockfd, recvline, sizeof(recvline)) == 0) {
                err_quit("str_cli: server terminated prematurely");
            }
            Fputs(recvline, stdout);
        }

        if (FD_ISSET(fileno(fp), &rset)) {
            if (Fgets(sendline, MAXLINE, fp) == NULL) {
                return;
            }
            Writen(sockfd, sendline, strlen(sendline));
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
    // FILE *fp = fopen("./5-1cli.c", "rb");
    str_cli(stdin, sockfd);
    return 0;
}
