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

#define LENGTH_COMMAND 7
#define MSG_ENTER_COMMAND "Enter a command: "
#define MSG_ERROR_COMMAND "Unidentified command! "

// trimming \n
void trim_string (char *);

// connecting to the main chat
void main_chat();
// connecting to a group chat
void group_chat();
// creating a new group
void create_group();
// quitting from a group
void quit_group();

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
void help_change_message();
// calling help for choosing an object of an action
void help_choose();

#endif /* CLIENT_H */

