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
    
    printf (MSG_CONNECTING);
    while ((connect_status = connect_server()) != 0) {
        printf (MSG_ERR_CONNECTION);
        printf (MSG_ENTER_WHEN_ERR_CONNECTION);
        
        // choosing an action
        while (1) {
            if (fgets (command, LENGTH_COMMAND, stdin) == NULL) {
                printf (MSG_ERROR_ENTER);
                return (EXIT_FAILURE);
            }
            
            if (strcmp (command, "0") == 0)
                return (EXIT_SUCCESS);
            else if (strcmp (command, "1") == 0)
                break;
            else
                printf (MSG_ERROR_COMMAND);
            
            printf (MSG_ENTER_WHEN_ERR_CONNECTION);
        }
        
        printf (MSG_CONNECTING);
    }
    
    printf (MSG_SUCCESSFUL_CONNECTION);
    /*
    printf (MSG_ENTER_WHEN_CONNECTED);
    
    while (1) {
        if (fgets (command, LENGTH_COMMAND, stdin) == NULL) {
            printf (MSG_ERROR_ENTER);
            return (EXIT_FAILURE);
        }
        
        if (strcmp (command, "0") == 0)
            return (EXIT_SUCCESS);
        else if (strcmp (command, "1") == 0)
            login();
        else
            printf (MSG_ERROR_COMMAND);

        printf (MSG_ENTER_WHEN_CONNECTED);
    }
    */
    /*
    printf ("If you have an account, enter 1 and login\n");
    printf ("Otherwise, enter 2 to register a new account\n");
    printf ("If you want to leave this program, enter /exit\n");
    printf (MSG_ENTER_COMMAND);
    while (fgets (command, LENGTH_COMMAND, stdin) != NULL) {
        if (strcmp (command, "/exit") == 0)
            break;
        else {
            if (strcmp (command, "1") == 0)
                login();
            else if (strcmp (command, "2") == 0)
                registration();
            else
                printf (MSG_ERROR_COMMAND);
        }
        
    }
    */

    return (EXIT_SUCCESS);
}

