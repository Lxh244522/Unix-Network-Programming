#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int inet_pton(int family, const char *strptr, void *addstr)
{
    if (family == AF_INET) {
        struct in_addr in_val;

        if (inet_aton(strptr, &in_val)) {
            memcpy(addstr, &in_val, sizeof(struct in_addr));
            return 1;
        }
        return 0;
    }
    errno = EAFNOSUPPORT;
    return -1;
}

const char *inet_mntop(int family, const void *addptr, char *strptr, size_t len)
{
    const u_char *p = (const u_char*) addptr;
    if (family == AF_INET) {
        char temp[INET_ADDRSTRLEN];

        fprintf(stdout, "%d.%d.%d.%d\n", p[0], p[1], p[2], p[3]);
        snprintf(temp, sizeof(temp), "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
        if (strlen(temp) >= len) {
            errno = ENOSPC;
            return NULL;
        }
        strcpy(strptr, temp);
        return strptr;
    }
    errno = EAFNOSUPPORT;
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        return 1;
    }

    struct in_addr in_val, in_valc;
    if (inet_pton(AF_INET, argv[1], &in_val) == 1) {
        fprintf(stdout, "%d\n", in_val);
    } else
        return 1;

    in_valc = in_val;
    char strptr[INET_ADDRSTRLEN];
    if (inet_mntop(AF_INET, &in_val, strptr, INET_ADDRSTRLEN)) {
        fprintf(stdout, "%s\n", strptr);
    }

    return 0;
}
