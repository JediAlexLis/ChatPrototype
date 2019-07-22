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
int main (void) {
    
    int connect_status;
    
    // connecting to server
    printf (MSG_CONNECTING);
    refresh();
    
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
    
    /*
    if ((connect_status = connect_server()) == -1) {
        printf ("Something happened wrong!\n");
        return (EXIT_FAILURE);
    }
    */
    
    printf (MSG_SUCCESSFUL_CONNECTION);
    
    // initialize the screen
    initscr ();
    
    // enter the account
    int enter_status;
    if ((enter_status = enter_account()) == ERROR_INPUT) {      // error in input
        endwin ();
        printf (MSG_ERROR_ENTER);
        return (EXIT_FAILURE);
    } else if (enter_status == ERROR_CONNECTION) {              // error in connection
        endwin ();
        printf (MSG_DISCONNECTION);
        return (EXIT_FAILURE);
    } else if (enter_status == 0) {                             // to exit the program
        endwin ();
        return (EXIT_SUCCESS);
    }
    
    // enter the main menu
    int main_menu_status;
    if ((main_menu_status = main_menu()) == ERROR_THREAD) { // error in threads
        endwin ();
        printf (MSG_ERROR_THREADS);
        return (EXIT_FAILURE);
    } else if (main_menu_status == ERROR_INPUT) {           // error in input
        endwin ();
        printf (MSG_ERROR_ENTER);
        return (EXIT_FAILURE);
    } else if (main_menu_status == ERROR_CONNECTION) {      // error in connection
        endwin ();
        printf (MSG_DISCONNECTION);
        return (EXIT_FAILURE);
    }
    
    /*
    else if (main_menu_status == 0) {                 // to exit the program
        endwin ();
        return (EXIT_SUCCESS);
    }
    */
    
    endwin ();
    return (EXIT_SUCCESS);
}

