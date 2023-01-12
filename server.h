#ifndef SERVER_H
#define SERVER_H

#define BACKLOG 10

#include "interface.h"
#include <stdbool.h>

typedef struct s_Room
{
    int32_t id; // it's id is the client cfd + room_count that is a uint16_t
    int8_t clients_num;
    int clients[MAX_CLIENTS_ROOM];
    char name[MAX_ROOM_NAME];

} s_Room;

static int send_name_request(int cfd);
static void list_all_addresses(struct addrinfo*);
static int receive_data(int fd, char* msg);
static int get_listener_socket(void);
static int add_client_to_room(s_Room*, int);
static int retrieve_client_info(int cfd, s_Client*);
static s_Room* get_room(int32_t, s_Room**, int32_t); // full id, so it has to be assembled before being passed here
static int send_to_all(char msg[MAX_MESSAGE_DATA_LEN], int32_t room_id);
static int32_t make_room(int cfd); // and then id will be calculated based on cfd - there should be a function that counts how many rooms one client has made:
//                                It gets id of the client, and then gets the highest value of the last two bytes

static uint16_t count_user_rooms(int cfd, s_Room** all_rooms, int32_t room_count);
static bool register_room(s_Room*, uint32_t*);

#endif /* SERVER_H */
