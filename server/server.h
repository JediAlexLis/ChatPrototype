/* 
 * File:   server.h
 * Author: Alexander Lisitzin
 *
 * Created on July 17, 2019, 10:36 PM
 */

#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <errno.h>
#include <limits.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#define DATABASE "database.xml"
#define LENGTH_NICKNAME 15
#define LENGTH_PASSWORD 30
#define LENGTH_COMMAND 7
#define MSG_ERROR_THREADS "Error in creating of thread! Press Enter to close program. "
#define LENGTH_BUFFER 128

// structure of a message
typedef struct message_s {
    uint8_t type;
    char buffer[LENGTH_BUFFER];    
} message_t;

typedef struct client_node {
    int socket_id;
    struct client_node* prev;
    struct client_node* next;
    char name[15];
    char password[30];
    int user_id;
} client_list;

extern int exit_flag;
extern int server_sock;
extern struct sockaddr_in server_addr;
extern int threads_number;
extern client_list *now;
extern xmlDocPtr doc;

// handler for entering commands
void command_handler ();
// handler for connecting of clients
void client_connecting_handler ();
// handler for processing of requests
void client_requests_handler (void *client_sock);

message_t login_request (char *);
message_t register_request (char *);

//// processing a request for connecting to the main chat
//void main_chat_request (int);
//// processing a request for connecting to a group chat
//void group_chat_request (int);
//// processing a request for sending a message
//void send_message_request (char *);
//// processing a request for sending a file
//void send_file_request ();
//// processing a request for changing a message
//void change_message_request (int);
//// processing a request for creating a group
//void create_group_request ();
//// processing a request for inviting a new member to a group
//void invite_group_request ();
//// processing a request for quitting from a group
//void quit_group_request (int);
//
//// writing a message to log-file
//void write_log (const char *message);
//
client_list *new_node (int);

void str_overwrite_stdout();
void trim_string (char * string);

#endif /* SERVER_H */

