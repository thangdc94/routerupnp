/**
 * @file mq_sysv_interface.c
 * @brief A Message Queue interface based on System V lib
 * @details Message queue interface for quick communicate with other processes.
 * This uses System V message queue IPC.
 * 
 * @author Pham Ngoc Thang (thangdc94)
 * @bug No known bug
 */

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "mq_interface.h"
#include "logutil.h"
#include "util.h"

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

/** Max size of a message to send */
#define MAX_MSG_SIZE 128

/** Size of Receive Message Buffer  */
#define MSG_BUFFER_SIZE 512

/** Message Queue id for this process */
static int g_msqid;

int mqInterface_create()
{
    // Create a new queue with permission if doesn't exist. If already exist just use it
    int msgflg = IPC_CREAT | QUEUE_PERMISSIONS;

    // generate server key
    key_t key = ftok(QUEUE_NAME, 1);

    if ((g_msqid = msgget(key, msgflg)) < 0)
    {
        LOG(LOG_ERR, "Server: msgget (server):%s", strerror(errno));
        return -1;
    }
    LOG(LOG_DBG, "mqInterface_create success msqid_server = %d", g_msqid);
    return 0;
}

int mqInterface_send(const char *message, int pid)
{
    size_t buf_length;

    // Declare message structure to send.
    struct
    {
        long mtype;
        char mtext[MAX_MSG_SIZE];
    } sbuf;
    int msqid_client;

    // generate key
    key_t key = ftok(QUEUE_NAME, pid);

    // dont create a new queue, just get it to send data to client
    if ((msqid_client = msgget(key, QUEUE_PERMISSIONS)) < 0)
    {
        LOG(LOG_ERR, "Server: msgget (client):%s", strerror(errno));
        return -1;
    }
    LOG(LOG_DBG, "msgget: msgget succeeded: msqid_client = %d", msqid_client);
    sbuf.mtype = 1;
    strcpy(sbuf.mtext, message);
    buf_length = strlen(sbuf.mtext) + 1;

    /*
     * Send a message.
     */
    if (msgsnd(msqid_client, &sbuf, buf_length, IPC_NOWAIT) < 0)
    {
        LOG(LOG_ERR, "Server: Unable to send message to client: %s", strerror(errno));
        return -1;
    }
    LOG(LOG_DBG, "mqInterface_send success");
    return 0;
}

int mqInterface_receive(char **msg_ptr)
{
    int ret = 0;
    // Declare message structure for received message.
    struct
    {
        long mtype;
        char mtext[MSG_BUFFER_SIZE];
    } rbuf;
    /*
     * Receive an answer of message type 1.
     */
    if ((ret = msgrcv(g_msqid, &rbuf, MSG_BUFFER_SIZE, MESSAGE_TYPE, IPC_NOWAIT)) < 0)
    {
        if (errno != ENOMSG)
        {
            LOG(LOG_ERR, "Server: msgrcv failed: %s", strerror(errno));
        }
        return -1;
    }
    LOG(LOG_DBG, "mqInterface_receive success");
    *msg_ptr = (char *)calloc(MSG_BUFFER_SIZE, sizeof(char));
    strcpy(*msg_ptr, rbuf.mtext);
    return 0;
}

int mqInterface_destroy()
{
    if (msgctl(g_msqid, IPC_RMID, NULL) == -1)
    {
        LOG(LOG_ERR, "Message queue could not be deleted");
        return -1;
    }

    LOG(LOG_DBG, "Message queue was deleted");
    return 0;
}
