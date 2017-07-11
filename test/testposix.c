/**
 * @file testposix.c
 * @brief Application to test POSIX message queue
 * @details test Communicate with routerupnp process over POSIX message queue.
 * 
 * @author Pham Ngoc Thang (thangdc94)
 * @bug No known bug
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

/** Server queue name */
#define SERVER_QUEUE_NAME "/routerupnp-server"

/** Client queue name */
#define CLIENT_QUEUE_PREFIX "/routerupnp-client"

/** Permission for message queue */
#define QUEUE_PERMISSIONS 0660

/** Max message can be store in message queue */
#define MAX_MESSAGES 10

/** Max size of a message can be store in message queue */
#define MAX_MSG_SIZE 512

/** Size of Receive Message Buffer  */
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

void strfmt(char **strout, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    *strout = malloc(n + 1);
    va_start(args, fmt);
    vsprintf(*strout, fmt, args);
    va_end(args);
}

int main(int argc, char **argv)
{
    char client_queue_name[64];
    mqd_t qd_server, qd_client; // queue descriptors

    // create the client queue for receiving messages from server
    sprintf(client_queue_name, "%s-%d", CLIENT_QUEUE_PREFIX, getpid());

    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    if ((qd_client = mq_open(client_queue_name, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1)
    {
        perror("Client: mq_open (client)");
        exit(1);
    }

    if ((qd_server = mq_open(SERVER_QUEUE_NAME, O_WRONLY)) == -1)
    {
        perror("Client: mq_open (server)");
        exit(1);
    }

    char in_buffer[MSG_BUFFER_SIZE];

    printf("Ask for a token (Press <ENTER>): ");

    char temp_buf[10];
    char *data_test = "{\"enable\":true,\"rules\":[{\"eport\":\"9999\",\"iport\":\"9999\",\"proto\":\"UDP\"}]}";

    while (fgets(temp_buf, 2, stdin))
    {
        char *request_msg;
        strfmt(&request_msg, "{\"pid\":%d, \"data\":%s}", getpid(), data_test);

        // send message to server
        if (mq_send(qd_server, request_msg, strlen(request_msg), 0) == -1)
        {
            free(request_msg);
            perror("Client: Not able to send message to server");
            continue;
        }
        else
        {
            free(request_msg);
        }

        // receive response from server
        memset(in_buffer, 0, strlen(in_buffer));
        if (mq_receive(qd_client, in_buffer, MSG_BUFFER_SIZE, NULL) == -1)
        {
            perror("Client: mq_receive");
            exit(1);
        }
        // display token received from server
        printf("Client: Token received from server: %s\n\n", in_buffer);

        printf("Ask for a token (Press ): ");
    }

    if (mq_close(qd_client) == -1)
    {
        perror("Client: mq_close");
        exit(1);
    }

    if (mq_unlink(client_queue_name) == -1)
    {
        perror("Client: mq_unlink");
        exit(1);
    }
    printf("Client: bye\n");

    exit(0);
}