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
#include <sys/sendfile.h>

#define DATABASE "database.xml"
#define LENGTH_NICKNAME 15
#define LENGTH_PASSWORD 30
#define LENGTH_CHAT_MESSAGE 100
#define LENGTH_COMMAND 7

#define MSG_ERROR_THREADS "Error in creating of thread!\n"
#define MSG_ERROR_CONNECTION "Error in connection!\n"
#define MSG_ERROR_WORK_XML "Error in working with XML file!\n"

#define LENGTH_BUFFER 128

#define RESPONSE_CHAT_MESSAGE 7

// structure of a message
typedef struct message_s {
    uint8_t type;
    char buffer[LENGTH_BUFFER];    
} message_t;

typedef struct client_node {
    int socket_fd;
    struct client_node* prev;
    struct client_node* next;
    char name[LENGTH_NICKNAME];
    char ip[16];
    int user_id;
} client_list;

extern int exit_flag;
extern int server_sock;
extern struct sockaddr_in client_info;
extern int requests_number;
extern client_list *last_client;
extern client_list *first_client;
extern xmlDocPtr doc;

// handler for entering commands
void command_handler ();
// handler for connecting of clients
void client_connecting_handler ();
// handler for processing of requests
void client_requests_handler (void *);

// processor of the request to log in
message_t* login_request (char *, client_list*);
// processor of the request to register
message_t* register_request (char *);

// processing a request for connecting to the main chat
message_t* main_chat_request (client_list*);
//// processing a request for connecting to a group chat
//void group_chat_request (int);
//// processing a request for sending a message
message_t* send_message_request (client_list*, char *);
//// processing a request for sending a file
//void send_file_request ();
// processing a request for changing a message
message_t* change_message_request (client_list *, char *);
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
client_list *new_node (int, char *);

void str_overwrite_stdout();
void trim_string (char * string);

#endif /* SERVER_H */

