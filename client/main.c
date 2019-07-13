/* 
 * File:   main.c
 * Author: Alexander Lisitzin
 *
 * Created on July 11, 2019, 8:30 AM
 */

#include "client.h"

int sock;
char command[LENGTH_COMMAND] = {};

// entrance to the program
int main(void) {
    int connect_status;
    
    // connection to server
    printf (MSG_CONNECTING);
    while ((connect_status = connect_server()) != 0) { // == 0 means it connected successful
        printf (MSG_ERR_CONNECTION);
        printf (MSG_ENTER_WHEN_ERR_CONNECTION);
        
        // choosing an action
        while (1) {
            if (fgets (command, LENGTH_COMMAND, stdin) == NULL) {
                printf (MSG_ERROR_ENTER);
                return (EXIT_FAILURE);
            }
            
            trim_string (command);
            
            if (strcmp (command, "0") == 0) // exit
                return (EXIT_SUCCESS);
            else if (strcmp (command, "1") == 0)    // try again
                break;
            else
                printf (MSG_ERROR_COMMAND);
            
            printf (MSG_ENTER_WHEN_ERR_CONNECTION);
        }
        
        printf (MSG_CONNECTING);
    }
    
    printf (MSG_SUCCESSFUL_CONNECTION);
    
    printf (MSG_ENTER_WHEN_CONNECTED);
    
    // login or registration
    int login_status, registration_status;
    while (1) {
        if (fgets (command, LENGTH_COMMAND, stdin) == NULL) {
            printf (MSG_ERROR_ENTER);
            return (EXIT_FAILURE);
        }

        trim_string (command);
        
        if (strcmp (command, "0") == 0)
            return (EXIT_SUCCESS);
        else if (strcmp (command, "1") == 0) {
            if ((login_status = login()) == 1)  // == 1 - logged in successfully
                break;
            else if (login_status == -1) { // == -1 - logged in unsuccessfully
                printf (MSG_ERROR);
                return (EXIT_FAILURE);
            }
        } else if (strcmp (command, "2") == 0) {
            if ((registration_status = registration()) == -1) {  // == 1 - registered unsuccessfully
                printf (MSG_DISCONNECTION);
                return (EXIT_FAILURE);
            }
        } else
            printf (MSG_ERROR_COMMAND);

        printf (MSG_ENTER_WHEN_CONNECTED);
    }
    
    // enter the main menu
    main_menu();

    return (EXIT_SUCCESS);
}

