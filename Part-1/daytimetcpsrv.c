#include "unp.h"
#include <time.h>

int main(int argc, char *argv[])
{
	int listenfd, connfd;
	struct sockaddr_in servaddr;
	char buff[MAXLINE];
	time_t ticks;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	// init the contents of servaddr to 0
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(13);

	Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	while (1) {
		// return connected descriptor when tcp three-way handshake is finished and success
		connfd = Accept(listenfd, (SA *)NULL, NULL);

		// get time
		ticks = time(NULL);
		// added "%.24s\r\n" at the end of the time string and write to buff
		snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
		// write buff to client
		Write(connfd, buff, strlen(buff));

		// close the connection
		Close(connfd);
	}

}