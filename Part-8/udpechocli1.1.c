#include "unp.h"

void dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen)
{
    int n;
    char sendline[MAXLINE], recvline[MAXLINE];

    Connect(sockfd, (SA *)pservaddr, servlen);

    while (Fgets(sendline, MAXLINE, fp) != NULL) {
        Write(sockfd, sendline, strlen(sendline));

        n = Read(sockfd, recvline, sizeof(recvline));
        recvline[n] = 0;
        Fputs(recvline, stdout);
    }
}

int main(int argc, char const *argv[])
{
    int sockfd;
    struct sockaddr_in servaddr;

    if (argc != 2)
        err_quit("usage: udpcli <IPaddress>");

    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    dg_cli(stdin, sockfd, (SA *) &servaddr, sizeof(servaddr));
    return 0;
}
