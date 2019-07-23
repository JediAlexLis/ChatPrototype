/* 
 * File:   client.h
 * Author: Alexander Lisitzin
 *
 * Created on July 11, 2019, 8:31 AM
 */

#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <pthread.h>
#include <ncurses.h>
#include <arpa/inet.h>
#include <errno.h>
#include <limits.h>

#define LENGTH_NICKNAME 15
#define LENGTH_PASSWORD 30
#define LENGTH_COMMAND 7
#define LENGTH_RESPONSE_MESSAGE 100
#define LENGTH_BUFFER 128

#define ERROR_THREAD -3
#define ERROR_INPUT -2
#define ERROR_CONNECTION -1
#define CANCEL 0

#define TYPE_CHAT_RECEIVE (uint8_t) 1
#define TYPE_CHAT_SEND (uint8_t) 2

#define MSG_CONNECTING "Connecting to the server...\n"
#define MSG_ERROR_CONNECTION "It's unable to connect to the server!\n"
#define MSG_ENTER_WHEN_ERR_CONNECTION "Choose an option - 1 to try again or 0 to exit: "
#define MSG_ERROR_ENTER "Error in enter a command! Closing the program!\n"
#define MSG_ERROR_THREADS "Error in creating of thread! Closing the program!\n"
#define MSG_SUCCESSFUL_CONNECTION "You're connected to the server successfully!\n"
#define MSG_ENTER_WHEN_CONNECTED "Choose an option - 1 to login, 2 to register or 0 to exit: "
#define MSG_DISCONNECTION "It's disconnection from the server!\nNow forced shutdown of the program will happen!\n"
#define MSG_ERROR "Something happened wrong!\n"
#define MSG_ENTER_COMMAND "Enter a command: "
#define MSG_ERROR_COMMAND "Unidentified command! Try again\n"

// socket descriptor
extern int sock;
// variable for containing a command
extern char command[LENGTH_COMMAND];

// structure of a message
typedef struct message_s {
    uint8_t type;
    char buffer[LENGTH_BUFFER];    
} message_t;

// trimming \n
void trim_string (char *);

// connecting to the server
int connect_server();
// enter an account
int enter_account();
// log in
int login();
// registration
int registration();

// main menu
int main_menu();

// connecting to the main chat
int main_chat();
// connecting to a group chat
int group_chat();
// creating a new group
int create_group();
// quitting from a group
int quit_group();

// function for a thread of sending requests
void* send_request();
// function for a thread of receiving responses
void* recv_response();

// sending a message
void send_message();
// sending a file
void send_file();
// changing a message
void change_message();
// deleting a message
void delete_message();

// inviting a new member of a group
void invite_member();

// choosing an object of an action
void choose();

// calling help for main menu
void help_main_menu();
// calling help for a main chat
void help_main_chat();
// calling help for a group chat
void help_group_chat();
// calling help for sending messages
void help_send_message();
// calling help for sending files
void help_send_file();
// calling help for changing messages
void help_change_message();
// calling help for deleting messages
void help_delete_message();
// calling help for inviting of new members of a group
void help_invite_member();
// calling help for choosing an object of an action
void help_choose();

#endif /* CLIENT_H */

