/* 
    Header file for server.
    MIT License...
*/
#define BACKLOG 10

static int send_name_request(int cfd);
static void list_all_addresses(struct addrinfo*);

int receive_message(int fd, char* msg);
int create_server_socket(struct addrinfo hints, struct addrinfo* response);
