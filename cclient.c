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
#include "cclient.h"

struct addrinfo s_Hints;
struct addrinfo* s_ClientInfo;
char client_name[MAX_NAME_LEN]; // shouldn't be global - maybe pass a pointer to struct 
int main(int argc, const char* argv[])
{
    setbuf(stdout, NULL);

    memset(&s_Hints, 0, sizeof(s_Hints));
    s_Hints.ai_flags = AI_PASSIVE;
    s_Hints.ai_family = AF_INET;
    s_Hints.ai_socktype = SOCK_STREAM;

    printf("Enter your name:\n");
    fgets(client_name, MAX_NAME_LEN, stdin);

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

    uint8_t buff[MAX_MESSAGE_DATA_LEN];
    int bytes_received = 0;
    while (1)
    {
        bytes_received = recv(cfd, &buff, MAX_MESSAGE_DATA_LEN, 0);
        if (bytes_received < 0)
        {
            fprintf(stderr, "Error occured while receiving message.");
            close(cfd);
            exit(-1);
        } else if (bytes_received > 0)
        {
            process_message(buff, cfd);
        }
    }
    close(cfd);
    return 0;
}

int process_message(uint8_t* msg_in_buffer, int cfd)
{
    uint8_t msg_out_buffer[MAX_MESSAGE_LEN];
    uint16_t response_id;

    uint16_t num_id = (msg_in_buffer[0] << 8) | msg_in_buffer[1];
    MessageId id = (MessageId)num_id;
    fprintf(stdout, "Message Id is: %d\n", num_id);

    switch (id)
    {
        case NAME_REQUEST:
        {
            fprintf(stdout, "Received a request for name\n.");
            response_id = (uint16_t)NAME_SET;
            msg_out_buffer[0] = ((uint16_t)NAME_SET >> 8);
            msg_out_buffer[1] = ((uint16_t)NAME_SET & 0xFF);
            int name_len = strlen(client_name);
            
            if (strlcpy((char*)msg_out_buffer + 2, client_name, name_len) <= 0)
            {
                fprintf(stderr, "Couldn't copy name to an out buffer.\n");
                exit(-1);
            } 

            for (int i = 0; i < MAX_MESSAGE_DATA_LEN - name_len; i++)
            {
                msg_out_buffer[2 + name_len] = 0;
            }
            close_on_fail(send(cfd, msg_out_buffer, MAX_MESSAGE_LEN, 0), "Couldn't send client's name as response.", cfd);
        }
        default:
        {
        }
    }

    return 0;
}