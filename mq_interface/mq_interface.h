/**
 * @file mq_interface.h
 * @brief A Message Queue interface 
 * @details Message queue interface for quick communicate with other processes.
 * This file contains function prototypes for Message queue communication.
 * 
 * @author Pham Ngoc Thang (thangdc94)
 * @bug No known bug
 */

#ifndef __MQ_INTERFACE_H_
#define __MQ_INTERFACE_H_

/**
 * @brief Create Message Queue Interface
 * @details Setup parameter for POSIX Message Queue and Create a Message Queue
 * @return 0 if OK or error code < 0 if failed
 */
int mqInterface_create();

/**
 * @brief Send message to a client which has specfic pid
 * @details Send message to message queue of client.
 * 
 * @param[in] message message content
 * @param[in] pid process id of client. Your client need to send pid first
 * in its message
 * 
 * @return 0 if OK or error code < 0 if failed
 */
int mqInterface_send(const char *message, int pid);

/**
 * @brief Receive message from client
 * @details Message from client will send to message queue. We will read
 * message from this message queue.
 * 
 * @param[out] msg_ptr pointer to message buffer. 
 * @warning You need to free() @p msg_ptr after use it.
 * @return 0 if OK or error code < 0 if failed
 */
int mqInterface_receive(char **msg_ptr);

/**
 * @brief Destroy message queue
 * @details Removes the association between message queue descriptor and its
 * message queue, then remove message queue
 * @return 0 if OK or error code < 0 if failed
 */
int mqInterface_destroy();
#endif //__MQ_INTERFACE_H_
