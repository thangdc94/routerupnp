/**
 * @file testsysv.c
 * @brief Application to test System V message queue
 * @details test Communicate with routerupnp process over System V message queue.
 * 
 * @author Pham Ngoc Thang (thangdc94)
 * @bug No known bug
 */

#include <sys/types.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * pathname (which must refer to an existing, accessible file or directory) to 
 * generate key for queue 
 */
#define QUEUE_NAME "/tmp"

/**
 * Type of message (msgtyp) <br>
 * If msgtyp == 0, then the first message in the queue is read. <br>
 * If msgtyp > 0, then the first message in the queue of type msgtyp is read. <br>
 * If msgtyp < 0, then the first message in the queue with the lowest type less
 * than or equal to the absolute value of msgtyp will be read. <br>
 */
#define MESSAGE_TYPE 1

/** Permission for message queue */
#define QUEUE_PERMISSIONS 0660

/** Max message can be store in message queue */
#define MAX_MESSAGES 10

/** Max size of a message can be store in message queue */
#define MAX_MSG_SIZE 512

/** Size of Receive Message Buffer  */
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

/** Declare the message structure */
typedef struct msgbuf
{
    long mtype;                  /**< message type */
    char mtext[MSG_BUFFER_SIZE]; /**< message content*/
} message_buf;

static void strfmt(char **strout, const char *fmt, ...)
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

/**
 * @brief Main function
 * @details You know it's a main function
 * 
 * @param[in] argc Argument count. We don't use it
 * @param[in] argv Argument variables. We don't use it too 
 * 
 * @return Error code or 0 if OK
 */
int main(int argc, char **argv)
{
    int msqid_server, msqid_client;
    key_t key_server, key_client;
    message_buf sbuf, rbuf;
    size_t buf_length;

    char c;

    key_server = ftok(QUEUE_NAME, 1);

    fprintf(stderr, "msgget: Calling msgget(%d,\%#o)\n", key_server, QUEUE_PERMISSIONS);

    if ((msqid_server = msgget(key_server, QUEUE_PERMISSIONS)) < 0)
    {
        perror("msgget");
        exit(1);
    }
    else
    {
        fprintf(stderr, "msgget: msgget succeeded: msqid_server = %d\n", msqid_server);
    }
    printf("pid = %d\n", getpid());
    key_client = ftok(QUEUE_NAME, getpid());

    fprintf(stderr, "\nmsgget: Calling msgget(%d,\%#o)\n", key_client, IPC_CREAT | QUEUE_PERMISSIONS);

    if ((msqid_client = msgget(key_client, IPC_CREAT | QUEUE_PERMISSIONS)) < 0)
    {
        perror("msgget");
        exit(1);
    }
    else
    {
        fprintf(stderr, "msgget: msgget succeeded: msqid_client = %d\n", msqid_client);
    }

    /*
     * We'll send message type 1
     */

    sbuf.mtype = MESSAGE_TYPE;

    char *request_msg;

    /*
     * Send a message.
     */
    while ((c = getchar()) != 'q')
    {
        if (c == 'd')
        {
            char *data_test = "{\"enable\":false,\"rules\":[{\"eport\":\"9999\",\"iport\":\"9999\",\"proto\":\"UDP\"}]}";
            strfmt(&request_msg, "{\"pid\":%d, \"data\":%s}", getpid(), data_test);
        }
        else
        {
            char *data_test = "{\"enable\":true,\"rules\":[{\"eport\":\"9999\",\"iport\":\"9999\",\"proto\":\"UDP\"}]}";
            strfmt(&request_msg, "{\"pid\":%d, \"data\":%s}", getpid(), data_test);
        }
        strcpy(sbuf.mtext, request_msg);
        buf_length = strlen(sbuf.mtext) + 1;
        // send message to server
        if (msgsnd(msqid_server, &sbuf, buf_length, IPC_NOWAIT) < 0)
        {
            perror("msgsnd");
            exit(1);
        }
        else
        {
            printf("Message: \"%s\" Sent\n", sbuf.mtext);
        }
        // receive response from server
        if (msgrcv(msqid_client, &rbuf, MSG_BUFFER_SIZE, MESSAGE_TYPE, 0) < 0)
        {
            perror("Client: msgrcv");
            exit(1);
        }
        // display token received from server
        printf("Client: Token received from server: %s\n\n", rbuf.mtext);
    }
}