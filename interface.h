#define MAX_NAME_LEN 16
#define PORT "2222"
#define MAX_MESSAGE_DATA_LEN 64
#define MAX_MESSAGE_LEN 66
#define MAX_CLIENTS 10

typedef enum MessageId
{
    NAME_REQUEST = 0xffff,
    NAME_SET = 0xfffe,
    SEND_MESSAGE = 0xfffd,
    CONNECT_ROOM = 0xfffc,
    MAKE_ROOM = 0xfffb,
} MessageId;

/* Struct holding the client data on server side */
typedef struct s_Client
{   
    uint16_t id;
    int cfd;
    char username[16];
} s_Client;

typedef struct s_Message
{
    uint16_t id;
    // Shouldn't there be at least a sender fd included?
    uint8_t data[MAX_MESSAGE_DATA_LEN];
} s_Message;

// Maybe do make_message(id, *data) that returns a structure
// 

// Fail if problem with sending function

void close_on_fail(ssize_t bytes, const char* msg, int fd);
