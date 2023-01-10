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
#include <poll.h>
#include "server.h"
#include "interface.h"

struct addrinfo s_Hints;
struct addrinfo* s_ServerInfo;
int main(int argc, const char* argv[])
{
    setbuf(stdout, NULL);

    // memset(&s_Hints, 0, sizeof(s_Hints));
    // s_Hints.ai_flags = AI_PASSIVE;
    // s_Hints.ai_family = AF_UNSPEC;
    // s_Hints.ai_socktype = SOCK_STREAM;

    // int addrinfo_status = getaddrinfo("192.168.1.21", PORT, &s_Hints, &s_ServerInfo);
    // //list_all_addresses(s_Response);

    // if (addrinfo_status > 0)
    // {
    //     fprintf(stderr, "An error occured:\n %s", gai_strerror(addrinfo_status));
    //     return -1;
    // }

    int server_sfd = get_listener_socket();
    if (server_sfd < 0)
    {
        fprintf(stderr, "An error occured while creating a socket.");
        return -1;
    }

    // int bind_status = bind(server_sfd, s_ServerInfo->ai_addr, s_ServerInfo->ai_addrlen);
    // if (bind_status < 0)
    // {
    //     fprintf(stderr, "An error occured while binding the socket: %s", strerror(errno));
    //     return -1;
    // }

    // int listen_status = listen(server_sfd, BACKLOG);
    // if (listen_status < 0)
    // {
    //     fprintf(stderr, "An error occured while listening: %s", strerror(errno));
    //     return -1;
    // }

    //struct in_addr s_ia = ((struct sockaddr_in*)s_ServerInfo->ai_addr)->sin_addr;
    //fprintf(stdout, "Socket listening on port %s IP: %s\n", PORT, inet_ntoa(s_ia));
    //freeaddrinfo(s_Response); // do we need s_Response here?

    int client_sfd = 0;
    int bytes_sent = 0;
    int bytes_received = 0;
    unsigned int clients_num = 0;
    uint8_t message_buffer[MAX_MESSAGE_LEN];
    struct pollfd pfds[MAX_CLIENTS];
    struct s_Client client_arr[MAX_CLIENTS];
    struct sockaddr s_ClientResponse;
    socklen_t c_addr_len = sizeof(s_ClientResponse);

    struct pollfd listener_pfd = {.fd = server_sfd, .events = POLL_IN};
    pfds[clients_num] = listener_pfd;

    while (1)
    {
        if (poll(pfds, clients_num + 1, -1) == -1)
        {
            fprintf(stderr, "Error while polling.");
            exit(-1);
        }

        for (int i = 0; i < clients_num + 1; i++)
        {
            if (pfds[i].revents & POLL_IN) // there's data to read
            {
                if (pfds[i].fd == server_sfd) // new connection
                {
                    fprintf(stdout, "New connection.\n");
                    client_sfd = accept(server_sfd, &s_ClientResponse, &c_addr_len);

                    // add client ...
                    clients_num++;
                    struct pollfd client_pfd = {.fd = client_sfd, .events = POLL_IN};
                    pfds[clients_num] = client_pfd;

                    if (client_sfd < 0)
                    {
                        fprintf(stderr, "Error while accepting new connection\n.");
                    }
                    else 
                    {
                        bytes_sent = send_name_request(client_sfd);
                        // create event for sending.
                    }
                }
                else // client sent something 
                {
                    int client_sfd = pfds[i].fd;
                    bytes_received = recv(client_sfd, &message_buffer, MAX_MESSAGE_LEN, 0);
                    fprintf(stdout, "Client sending something...");
                    if (bytes_received > 0)
                    {
                        // TODO: Make function to handle receiving.
                        uint16_t num_id = (message_buffer[0] << 8) | message_buffer[1];
                        if (num_id == (uint16_t)NAME_SET)
                        {
                            fprintf(stdout, "Client's name:\n");
                            for (uint8_t* p = message_buffer + 2; *p != '\0'; p++)
                            {
                                fprintf(stdout, "%c", *p);
                            }
                        }
                    } else 
                    {
                        if (bytes_received == 0) // connection closed
                        {
                            // Handle disconnection
                            fprintf(stdout, "Client just disconnected.");
                            close(client_sfd);
                        } else 
                        {
                            fprintf(stderr, "Error while receiving data from client");
                        }
                    }
                }
            }
        }
        /* Client won't have to respond immediately with response NAME_SET */
    }

    // while (1)
    // {
    //     client_sfd = accept(server_sfd, &s_ClientResponse, &c_addr_len);
    //     if (client_sfd < 0) // error
    //     {
    //         /* Serve... */
    //         /* Listen for messages */
    //         /* Respond */
            
    //         continue;
        
    //     } else if (client_sfd == 0)
    //     {
    //     } 
    //     else // new connection
    //     {
    //         bytes_sent = send_name_request(client_sfd);
    //         bytes_received = recv(client_sfd, &message_buffer, MAX_MESSAGE_LEN, 0);

    //         if (bytes_received > 0)
    //         {
    //             uint16_t num_id = (message_buffer[0] << 8) | message_buffer[1];
    //             if (num_id == (uint16_t)NAME_SET)
    //             {
    //                 fprintf(stdout, "Client's name:\n");
    //                 for (uint8_t* p = message_buffer + 2; *p != '\0'; p++)
    //                 {
    //                     fprintf(stdout, "%c", *p);
    //                 }

    //                 // client has to respond immediately - if not, we have to track who sends NAME_SET here!

    //                 // Send message to all (in the same room), that a new client has connected!
    //             }
    //         } 
    //         else 
    //         {
    //             fprintf(stderr, "Couldn't receive name.\n");
    //         }

    //         // receive...

    //         // int8_t msg_data[] = "You've successfully connected to the server.";
    //         // // add message spliting in case of it being too big.
    //         // bytes_sent = send(client_sfd, &msg, MAX_MESSAGE_DATA_LEN, 0);


    //     }
    //     if (bytes_sent < 0)
    //     {
    //         fprintf(stderr, "An error occured while sending a message: ");
    //         fprintf(stderr, "%s", strerror(errno));
    //     }
    // }

    // close(server_sfd);
    return 0;
}

static void list_all_addresses(struct addrinfo* s_addressinfo)
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

int receive_data(int fd, char* msg)
{
    return 0;
}

static int send_name_request(int cfd) // TODO: Change caller to use function in interface.h
{
    uint8_t buffer[MAX_MESSAGE_LEN];
    buffer[0] = ((uint16_t)NAME_REQUEST >> 8);
    buffer[1] = ((uint16_t)NAME_REQUEST & 0xFF);

    for (int i = 0; i < MAX_MESSAGE_DATA_LEN; i++)
    {
        buffer[2 + i] = 0;
    }
    return send(cfd, &buffer, MAX_MESSAGE_LEN, 0);
}

int get_listener_socket(void)
{
    int listener;
    int yes = 1;
    struct addrinfo s_Hints;
    struct addrinfo* s_ServerInfo;

    memset(&s_Hints, 0, sizeof(s_Hints));
    s_Hints.ai_flags = AI_PASSIVE;
    s_Hints.ai_family = AF_UNSPEC;
    s_Hints.ai_socktype = SOCK_STREAM;

    int addrinfo_status = getaddrinfo("192.168.1.21", PORT, &s_Hints, &s_ServerInfo);
    if (addrinfo_status > 0)
    {
        fprintf(stderr, "An error occured:\n %s", gai_strerror(addrinfo_status));
        return -1;
    }

    listener = socket(s_ServerInfo->ai_family, s_ServerInfo->ai_socktype, s_ServerInfo->ai_protocol);
    if (listener < 0)
    {
        fprintf(stderr, "An error occured while creating a socket.");
        return -1;
    }

    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    int bind_status = bind(listener, s_ServerInfo->ai_addr, s_ServerInfo->ai_addrlen);
    if (bind_status < 0)
    {
        fprintf(stderr, "An error occured while binding the socket: %s", strerror(errno));
        return -1;
    }

    int listen_status = listen(listener, BACKLOG);
    if (listen_status < 0)
    {
        fprintf(stderr, "An error occured while listening: %s", strerror(errno));
        return -1;
    }

    struct in_addr s_ia = ((struct sockaddr_in*)s_ServerInfo->ai_addr)->sin_addr;
    fprintf(stdout, "Socket listening on port %s IP: %s\n", PORT, inet_ntoa(s_ia));
    freeaddrinfo(s_ServerInfo);
    return listener;
}
