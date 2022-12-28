/* 
    Header file for server.
    MIT License...
*/
#define BACKLOG 10


int create_server_socket(struct addrinfo hints, struct addrinfo* response);
void list_all_addresses(struct addrinfo*);
