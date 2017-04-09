#include "unp.h"

int main(int argc, char const *argv[])
{
    struct hostent *host;
    for (int i=1; i<argc; i++) {
        if ((host = gethostbyname(argv[i])) == NULL) {
            err_sys("gethostbyname error for host: %s: %s",
                    argv[i], hstrerror(h_errno));
            continue;
        }
        printf("official (canonical) hostname %s\n", host->h_name);

        char **pptr;
        char str[INET_ADDRSTRLEN];
        for (pptr=host->h_aliases; *pptr != NULL; pptr++)
            printf("\talias: %s\n", *pptr);

        switch (host->h_addrtype) {
            case AF_INET:
                pptr = host->h_addr_list;
                for (; *pptr != NULL; pptr++)
                    printf("\taddress: %s\n",
                            Inet_ntop(host->h_addrtype, *pptr, str, sizeof(str)));
                break;
            default:
                err_ret("unknown address type");
                break;
        }
    }
    return 0;
}
