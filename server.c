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
#include "server.h"
#include "interface.h"

struct addrinfo s_Hints;
struct addrinfo* s_ServerInfo;
int main(int argc, const char* argv[])
{
    setbuf(stdout, NULL);

    memset(&s_Hints, 0, sizeof(s_Hints));
    s_Hints.ai_flags = AI_PASSIVE;
    s_Hints.ai_family = AF_UNSPEC;
    s_Hints.ai_socktype = SOCK_STREAM;

    int addrinfo_status = getaddrinfo("192.168.1.21", PORT, &s_Hints, &s_ServerInfo);
    //list_all_addresses(s_Response);

    if (addrinfo_status > 0)
    {
        fprintf(stderr, "An error occured:\n %s", gai_strerror(addrinfo_status));
        return -1;
    }

    int server_sfd = socket(s_ServerInfo->ai_family, s_ServerInfo->ai_socktype, s_ServerInfo->ai_protocol);
    if (server_sfd < 0)
    {
        fprintf(stderr, "An error occured while creating a socket.");
        return -1;
    }

    int bind_status = bind(server_sfd, s_ServerInfo->ai_addr, s_ServerInfo->ai_addrlen);
    if (bind_status < 0)
    {
        fprintf(stderr, "An error occured while binding the socket: %s", strerror(errno));
        return -1;
    }

    int listen_status = listen(server_sfd, BACKLOG);
    if (listen_status < 0)
    {
        fprintf(stderr, "An error occured while listening: %s", strerror(errno));
        return -1;
    }

    struct in_addr s_ia = ((struct sockaddr_in*)s_ServerInfo->ai_addr)->sin_addr;
    fprintf(stdout, "Socket listening on port %s IP: %s\n", PORT, inet_ntoa(s_ia));
    //freeaddrinfo(s_Response); // do we need s_Response here?

    int client_sfd = 0;
    struct sockaddr s_ClientResponse;
    socklen_t c_addr_len = sizeof(s_ClientResponse);

    int bytes_sent = 0;

    while (1)
    {
        client_sfd = accept(server_sfd, &s_ClientResponse, &c_addr_len);
        if (client_sfd < 0)
        {
            continue;
        } else // new connection
        {
            // send to client our info and that he is connected.
            // recv name and add client.
            fprintf(stdout, "New client connected.\n");
            fprintf(stdout, "Sending data...");

            int8_t msg[] = "This is server, hi."; 
            bytes_sent = send(client_sfd, &msg, MAX_MESSAGE_LEN, 0);
            if (bytes_sent < 0)
            {
                fprintf(stderr, "An error occured while sending a message: ");
                fprintf(stderr, "%s", strerror(errno));
            }
        }
    }

    close(server_sfd);
    freeaddrinfo(s_ServerInfo);
    return 0;
}

void list_all_addresses(struct addrinfo* s_addressinfo)
{
    struct addrinfo* sp;
    void* address;

    for (sp = s_addressinfo; sp != NULL; sp = sp->ai_next)
    {
        if (sp->ai_family == AF_INET)
        {
            struct sockaddr_in* ipv4 = (struct sockaddr_in*)sp->ai_addr;
            address = &ipv4->sin_addr;
            fprintf(stdout, "IPv4: %s\n", inet_ntoa(*(struct in_addr*)address));
        } else if (sp->ai_family == AF_INET6)
        {
            char ipstr[INET6_ADDRSTRLEN];
            struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)sp->ai_addr;
            address = &ipv6->sin6_addr;
            fprintf(stdout, "IPv6: %s\n", inet_ntop(sp->ai_family, address, ipstr, sizeof(ipstr)));
        }
    }
}