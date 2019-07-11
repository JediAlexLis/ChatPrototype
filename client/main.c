/* 
 * File:   main.c
 * Author: Alexander Lisitzin
 *
 * Created on July 11, 2019, 8:30 AM
 */

#include "client.h"

// main menu
int main(void) {
    char command[LENGTH_COMMAND] = {};
    
    printf ("Welcome to ChatPrototype!\n");
    printf ("To find out which command is referred to which action, enter /help\n");
    printf (MSG_ENTER_COMMAND);
    
    while (fgets (command, LENGTH_COMMAND, stdin) != NULL) {
        trim_string(command);
        
        if (strcmp (command, "/help") == 0) {
            help_main_menu();
        } else if (strcmp (command, "/exit") == 0) {
            break;
        } else {
            if (strcmp (command, "1") == 0) {
                main_chat();
            } else if (strcmp (command, "2") == 0) {
                group_chat()
            } else if (strcmp (command, "3") == 0) {
                create_group();
            } else if (strcmp (command, "4") == 0) {
                quit_group();
            } else {
                printf (MSG_ERROR_COMMAND);
            }
        }
        
        printf (MSG_ENTER_COMMAND);
    }
    
    return (EXIT_SUCCESS);
}

