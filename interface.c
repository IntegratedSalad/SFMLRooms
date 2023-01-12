#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include "interface.h"

void close_on_fail(ssize_t bytes, const char* msg, int fd)
{
    if (bytes < 0)
    {
        fprintf(stderr, "%s", msg);
        close(fd);
        exit(-1);
    }
}

/* Send message NOT GUARANTEEING there is a null terminator at the end */
int send_message(const int fd, const MessageId id, const uint8_t data[MAX_MESSAGE_DATA_LEN], const uint8_t data_len)
{
    uint8_t msg[MAX_MESSAGE_LEN];
    uint16_t message_id_num = (uint16_t)id;
    msg[0] = (message_id_num >> 8);
    msg[1] = (message_id_num & 0xFF);

    if (strlcpy((char*)msg + 2, data, data_len) <= 0)
    {
        return -1;
    }

    if (data_len < (uint8_t)MAX_MESSAGE_DATA_LEN)
    {
        for (uint8_t* p = msg + 2 + data_len; p < msg + (MAX_MESSAGE_DATA_LEN - data_len); p++)
        {
            *p = 0; // zero the remaining bytes
        }
    }
    return send(fd, msg, MAX_MESSAGE_LEN, 0);
}
