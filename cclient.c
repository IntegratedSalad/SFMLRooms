#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "interface.h"

struct addrinfo s_Hints;
struct addrinfo* s_ClientInfo;
int main(int argc, const char* argv[])
{
    memset(&s_Hints, 0, sizeof(s_Hints));
    s_Hints.ai_flags = AI_PASSIVE;
    s_Hints.ai_family = AF_INET;
    s_Hints.ai_socktype = SOCK_STREAM;

    int addrinfo_status = getaddrinfo("192.168.1.21", PORT, &s_Hints, &s_ClientInfo);
    if (addrinfo_status < 0)
    {
        fprintf(stderr, "An error occured:\n %s", gai_strerror(addrinfo_status));
        return -1;
    }

    int cfd = socket(s_ClientInfo->ai_family, s_ClientInfo->ai_socktype, s_ClientInfo->ai_protocol);
    if (cfd < 0)
    {
        fprintf(stderr, "An error occured while creating a socket.");
        close(cfd);
        return -1;
    }

    int connection_status = connect(cfd, s_ClientInfo->ai_addr, s_ClientInfo->ai_addrlen);
    if (connection_status < 0)
    {
        fprintf(stderr, "An error occured while connecting to the remote host.");
        close(cfd);
        return -1;
    }

    int8_t buff[MAX_MESSAGE_LEN];
    int bytes_received = 0;
    
    bytes_received = recv(cfd, &buff, MAX_MESSAGE_LEN, 0);
    if (bytes_received < 0)
    {
        fprintf(stderr, "Error occured while receiving message.");
        close(cfd);
    } else 
    {
        fprintf(stdout, "%s", buff);
    }

    close(cfd);

    return 0;
}