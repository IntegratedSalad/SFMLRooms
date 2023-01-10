/* 
    Header file for server.
    MIT License...
*/
#define BACKLOG 10

static int send_name_request(int cfd);
static void list_all_addresses(struct addrinfo*);

static int receive_data(int fd, char* msg);
static int get_listener_socket(void);
