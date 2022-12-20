/* 
    Header file for server.
    MIT License...
*/
#define PORT "2222"
#define BACKLOG 10

typedef struct s_Client
{

} Client;

int create_server_socket(struct addrinfo hints, struct addrinfo* response);
void list_all_addresses(struct addrinfo*);
