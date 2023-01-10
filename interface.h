#define MAX_NAME_LEN 16
#define PORT "2222"
#define MAX_MESSAGE_DATA_LEN 64
#define MAX_MESSAGE_LEN 66
#define MAX_CLIENTS 10
#define MAX_CLIENTS_ROOM 4

typedef enum MessageId
{
    NAME_REQUEST = 0xffff,
    NAME_SET = 0xfffe,
    SEND_MESSAGE = 0xfffd, // general message - destination included in data
    CONNECT_ROOM = 0xfffc,
    MAKE_ROOM = 0xfffb,
    ROOM_INVITE = 0xfffa,
    ROOM_KICK = 0xffef
} MessageId;

/* Struct holding the client data on server side */
typedef struct s_Client
{   
    int cfd;
    char username[MAX_NAME_LEN];
} s_Client;

typedef struct s_Message
{
    uint16_t id;
    // Shouldn't there be at least a sender fd included?
    uint8_t data[MAX_MESSAGE_DATA_LEN];
} s_Message;

typedef struct s_Room
{
    uint16_t id; // it's id is the client cfd
    s_Client clients[MAX_CLIENTS_ROOM];

} s_Room;

// Fail if problem with sending function
void close_on_fail(ssize_t bytes, const char* msg, int fd);
int send_message(const int fd, const MessageId id, const uint8_t data[MAX_MESSAGE_DATA_LEN], const uint8_t data_len);
s_Message unpack_message(MessageId id, const uint8_t data[MAX_MESSAGE_DATA_LEN]);
