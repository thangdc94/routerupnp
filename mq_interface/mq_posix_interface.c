/**
 * @file mq_posix_interface.c
 * @brief A Message Queue interface based on POSIX lib
 * @details Message queue interface for quick communicate with other processes.
 * This uses POSIX message queue IPC.
 * 
 * @author Pham Ngoc Thang (thangdc94)
 * @bug No known bug
 */

#include <mqueue.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "mq_interface.h"
#include "logutil.h"
#include "util.h"

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

static mqd_t qd_server; // message queue descriptor

int mqInterface_create()
{
    // setup attributes
    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0; /* Number of messages currently queued. */

    if ((qd_server = mq_open(SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1)
    {
        LOG(LOG_ERR, "Server: mq_open (server)");
        return errno;
    }

    LOG(LOG_DBG, "mqInterface_create success");
    return 0;
}

int mqInterface_send(const char *message, int pid)
{
    mqd_t qd_client;
    char *queue_name;
    int ret = 0;

    strfmt(&queue_name, "%s-%d", CLIENT_QUEUE_PREFIX, pid);
    if ((qd_client = mq_open(queue_name, O_WRONLY)) == 1)
    {
        LOG(LOG_ERR, "Server: Not able to open client queue");
        ret = errno;
        goto out;
    }

    if (mq_send(qd_client, message, strlen(message), 0) == -1)
    {
        LOG(LOG_ERR, "Server: Not able to send message to client");
        ret = errno;
        goto out;
    }
out:
    free(queue_name);
    if (ret == 0)
    {
        LOG(LOG_DBG, "mqInterface_send success");
    }
    return ret;
}

int mqInterface_receive(char **msg_ptr)
{
    *msg_ptr = (char *)calloc(MSG_BUFFER_SIZE, sizeof(char));
    if (mq_receive(qd_server, *msg_ptr, MSG_BUFFER_SIZE, NULL) == -1)
    {
        LOG(LOG_ERR, "Server: mq_receive");
        return errno;
    }
    LOG(LOG_DBG, "mqInterface_receive success");
    return 0;
}

int mqInterface_destroy()
{
    if (mq_close(qd_server) == -1)
    {
        LOG(LOG_ERR, "Server: mq_close");
        return errno;
    }

    if (mq_unlink(SERVER_QUEUE_NAME) == -1)
    {
        LOG(LOG_ERR, "Server: mq_unlink");
        return errno;
    }
    LOG(LOG_DBG, "mqInterface_destroy success");
    return 0;
}
