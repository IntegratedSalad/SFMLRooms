#define MAX_NAME_LEN 16
#define PORT "2222"
#define MAX_MESSAGE_LEN 64

typedef struct s_Client
{
    int cfd;
    char* name[16];
} s_Client;
