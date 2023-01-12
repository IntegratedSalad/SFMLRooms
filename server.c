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

int main(int argc, const char* argv[])
{
    setbuf(stdout, NULL);

    int server_sfd = get_listener_socket();
    if (server_sfd < 0)
    {
        fprintf(stderr, "An error occured while creating a socket.");
        return -1;
    }

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

    s_Room s_GlobalRoom = {.id = 0};
    s_Room** rooms = malloc(sizeof(s_Room*) * 1);
    uint32_t room_count = 1;

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
                    client_sfd = accept(server_sfd, &s_ClientResponse, &c_addr_len);

                    if (client_sfd < 0)
                    {
                        fprintf(stderr, "Error while accepting new connection\n.");
                    }
                    else 
                    {
                        clients_num++;
                        struct pollfd client_pfd = {.fd = client_sfd, .events = POLL_IN};
                        pfds[clients_num] = client_pfd;
                        bytes_sent = send_name_request(client_sfd);
                        add_client_to_room()
                        
                    }
                }
                else // client sent something 
                {
                    int client_sfd = pfds[i].fd;
                    bytes_received = recv(client_sfd, &message_buffer, MAX_MESSAGE_LEN, 0);
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
    }
    close(server_sfd);
    free(rooms);
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

static int receive_data(int fd, char* msg)
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

static int get_listener_socket(void)
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

static uint16_t count_user_rooms(int cfd, s_Room** all_rooms, int32_t room_count)
{
    s_Room* rp;
    uint16_t user_rooms = 0;

    for (int i = 0; i < room_count; i++)
    {
        rp = all_rooms[i];
        uint16_t fd_to_compare = (rp->id >> 16);
        if (fd_to_compare == (uint16_t)cfd) // casting int32_t to uint16_t, but we know that we have a limit of clients that is less than MAX value of uint16_t
        {
            uint16_t count_to_compare = (rp->id & 0x0000FFFF);
            if (count_to_compare > user_rooms)
            {
                user_rooms = count_to_compare;
            }
        }
    }

    return user_rooms;
}

static s_Room* get_room(int32_t id, s_Room** all_rooms, int32_t room_count)
{
    s_Room* room = NULL;
    s_Room* rp;

    for (int i = 0; i < room_count; i++)
    {
        rp = all_rooms[i];
        if (rp->id == id)
        {
            room = rp;
            break;
        }
    }
    return room;
}

static int add_client_to_room(s_Room* room, int cfd)
{
    if (room->clients_num >= MAX_CLIENTS_ROOM) return 0;

    room->clients[room->clients_num] = cfd;
    room->clients_num++;

    return 1;
}
