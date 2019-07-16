#include "client.h"

int sock;
char command[LENGTH_COMMAND];

// trimming \n
void trim_string (char * string) {
    int i = strlen (string) - 1;
    if (string[i] == '\n') string [i] = '\0';
}

// connecting to the server
int connect_server() {
    
    printf ("0 - successful, -1 - unsuccessful: ");
    int result = -1;
    scanf ("%d", &result);
    getchar ();
    
    return result;
    
    /*
    // creating a socket
    if ((sock = socket (AF_INET, SOCK_STREAM, 0)) == -1)
        return -1;
    struct sockaddr_in server;
    
    // setting IP-address and port of the server
    memset ((char*) &server, '\0', sizeof (server));
    server.sin_family = PF_INET;
    server.sin_addr.s_addr = inet_addr ("127.0.0.1");
    server.sin_port = htons(8888);
    
    // connecting to the server by the socket
    return connect (sock, (struct sockaddr*) &server, sizeof (server));
    */
}
// enter an account
int enter_account() {
    WINDOW *enter_account_window = newwin (LINES, COLS, 0, 0);
    
    wprintw (enter_account_window, MSG_ENTER_WHEN_CONNECTED);
    refresh ();
    
    // choice of login or registration
    int login_status, registration_status;
    while (1) {
        if (wgetnstr (enter_account_window, command, LENGTH_COMMAND) == ERR) { // error with input
            delwin (enter_account_window);
            return ERROR_INPUT;
        }

        trim_string (command);
        
        if (strcmp (command, "0") == 0) {   // to exit the program
            delwin (enter_account_window);
            return 0;
        }
        else if (strcmp (command, "1") == 0) {  // to log in
            if ((login_status = login()) == ERROR_INPUT) {   // error in input
                delwin (enter_account_window);
                return ERROR_INPUT;
            } else if (login_status == ERROR_CONNECTION) {    // error in connection
                delwin (enter_account_window);
                return ERROR_CONNECTION;
            } else if (login_status == 1) {     // logged in successfully
                break;
            } else {                            // cancel of login
                delwin (enter_account_window);
                enter_account_window = newwin (LINES, COLS, 0, 0);
            }
        } else if (strcmp (command, "2") == 0) {    // to register
            if ((registration_status = registration()) == ERROR_INPUT) { // error in input
                delwin (enter_account_window);
                return ERROR_INPUT;
            } else if (registration_status == ERROR_CONNECTION) {             // error in connection
                delwin (enter_account_window);
                return ERROR_CONNECTION;
            } else {                                            // cancel of registration or registered successfully
                delwin (enter_account_window);
                enter_account_window = newwin (LINES, COLS, 0, 0);
            }
        } else {    // in the case when there is an unidentified command
            wprintw (enter_account_window, MSG_ERROR_COMMAND);
            // refresh();
        }

        wprintw (enter_account_window, MSG_ENTER_WHEN_CONNECTED);
        wrefresh (enter_account_window);
    }
    
    delwin (enter_account_window);
    return 1;
}
// log in
int login() {
    WINDOW *login_window = newwin (LINES, COLS, 0, 0);
    
    /*----------------------------*/
    wprintw (login_window, "1 - successfully, 0 - cancel, -1 - disconnection, -2 - error in input: ");
    wrefresh (login_window);
    
    int result;
    wscanw (login_window, "%d", &result);
    /*----------------------------*/
    
    delwin (login_window);
    
    return result;
}
// registration
int registration() {
    WINDOW *registration_window = newwin (LINES, COLS, 0, 0);
    
    /*----------------------------*/
    wprintw (registration_window, "1 - successfully, 0 - cancel, -1 - disconnection, -2 - error in input: ");
    wrefresh (registration_window);
    
    int result;
    wscanw (registration_window, "%d", &result);
    /*----------------------------*/
    
    delwin (registration_window);
    
    return result;
}

// main menu
int main_menu() {  
    WINDOW *main_menu_window = newwin (LINES, COLS, 0, 0);
    
    wprintw (main_menu_window, "Welcome to ChatPrototype!\n");
    wprintw (main_menu_window, "To find out which command is referred to which action, enter /help\n");
    wprintw (main_menu_window, MSG_ENTER_COMMAND);
    wrefresh (main_menu_window);
    
    int status;
    while (1) {
        if (wgetnstr (main_menu_window, command, LENGTH_COMMAND) == ERR) {  // error in input
            delwin (main_menu_window);
            return ERROR_INPUT;
        }
        
        trim_string(command);
        
        if (strcmp (command, "/help") == 0) {        // calling help
            help_main_menu();
            delwin (main_menu_window);
            main_menu_window = newwin (LINES, COLS, 0, 0);
            wprintw (main_menu_window, "You returned back to the main menu!\n");
            wprintw (main_menu_window, "To find out which command is referred to which action, enter /help\n");
        }
        else if (strcmp (command, "/exit") == 0)     // exit the program
            break;
        else {
            if (strcmp (command, "1") == 0) {   // enter the main chat
                if ((status = main_chat()) == ERROR_THREAD) {   // error in threads
                    delwin (main_menu_window);
                    return ERROR_THREAD;
                } else if (status == ERROR_INPUT) {         // error in input
                    delwin (main_menu_window);
                    return ERROR_INPUT;
                } else if (status == ERROR_CONNECTION) {    // error in connection
                    delwin (main_menu_window);
                    return ERROR_CONNECTION;
                } else {                                    // without error
                    delwin (main_menu_window);
                    main_menu_window = newwin(LINES, COLS, 0, 0);
                    wprintw (main_menu_window, "You returned back to the main menu!\n");
                    wprintw (main_menu_window, "To find out which command is referred to which action, enter /help\n");
                }
            }
            else if (strcmp (command, "2") == 0) {  // enter the group chat
                if ((status = group_chat()) == ERROR_THREAD) {  // error in threads
                    delwin (main_menu_window);
                    return ERROR_THREAD;
                } else if (status == ERROR_INPUT) {         // error in input
                    delwin (main_menu_window);
                    return ERROR_INPUT;
                } else if (status == ERROR_CONNECTION) {    // error in connection
                    delwin (main_menu_window);
                    return ERROR_CONNECTION;
                } else {                                    // without error
                    delwin (main_menu_window);
                    main_menu_window = newwin(LINES, COLS, 0, 0);
                    wprintw (main_menu_window, "You returned back to the main menu!\n");
                    wprintw (main_menu_window, "To find out which command is referred to which action, enter /help\n");
                }
            }
            else if (strcmp (command, "3") == 0)    // creating a new group
                if ((status = create_group()) == ERROR_INPUT) { // error in input
                    delwin (main_menu_window);
                    return ERROR_INPUT;
                } else if (status == ERROR_CONNECTION) {        // error in connection
                    delwin (main_menu_window);
                    return ERROR_CONNECTION;
                } else {                                        // without error
                    delwin (main_menu_window);
                    main_menu_window = newwin(LINES, COLS, 0, 0);
                    wprintw (main_menu_window, "You returned back to the main menu!\n");
                    wprintw (main_menu_window, "To find out which command is referred to which action, enter /help\n");
                }
            else if (strcmp (command, "4") == 0)    // quit a group
                if ((status = quit_group()) == ERROR_INPUT) {   // error in input
                    delwin (main_menu_window);
                    return -2;
                } else if (status == ERROR_CONNECTION) {        // error in connection
                    delwin (main_menu_window);
                    return -1;
                } else {                                        // without error
                    delwin (main_menu_window);
                    main_menu_window = newwin(LINES, COLS, 0, 0);
                    wprintw (main_menu_window, "You returned back to the main menu!\n");
                    wprintw (main_menu_window, "To find out which command is referred to which action, enter /help\n");
                }
            else
                wprintw (main_menu_window, MSG_ERROR_COMMAND);
        }
        
        wprintw (main_menu_window, MSG_ENTER_COMMAND);
        wrefresh (main_menu_window);
    }
    
    delwin (main_menu_window);
    
    return 0;
}

// connecting to the main chat
int main_chat() {
    WINDOW *main_chat_window = newwin (LINES, COLS, 0, 0);
    
    wprintw (main_chat_window, "0 - successfully, -1 - disconnection, -2 - error in input: ");
    wrefresh (main_chat_window);
    
    int result;
    wscanw (main_chat_window, "%d", &result);
    
    /*
    int send_request_status = 0, recv_response_status = 0;
    
    pthread_t send_request_thread;
    if (pthread_create(&send_request_thread, NULL, &send_request, &send_request_status) != 0) {
        return ERROR_THREAD;
    }

    pthread_t recv_response_thread;
    if (pthread_create(&recv_response_thread, NULL, &recv_response, &recv_response_status) != 0) {
        return ERROR_THREAD;
    }
    
    while (1) {
        if (send_request_status == ERROR_INPUT) {
            delwin (main_chat_window);
            return ERROR_INPUT;
        } else if (send_request_status == ERROR_CONNECTION || recv_response_status == ERROR_CONNECTION) {
            delwin (main_chat_window);
            return ERROR_CONNECTION;
        }
        
    delwin (main_chat_window);
    
    return 0;
    */
    
    delwin (main_chat_window);
    return result;
}
// connecting to a group chat
int group_chat() {
    WINDOW *group_chat_window = newwin (LINES, COLS, 0, 0);
    
    /*----------------------------*/
    wprintw (group_chat_window, "0 - successfully, -1 - disconnection, -2 - error in input: ");
    wrefresh (group_chat_window);
    
    int result;
    wscanw (group_chat_window, "%d", &result);
    /*----------------------------*/
    
    delwin (group_chat_window);
    
    return result;
}
// creating a new group
int create_group() {
    WINDOW *create_group_win = newwin (LINES, COLS, 0, 0);
    
    /*----------------------------*/
    wprintw (create_group_win, "0 - successfully, -1 - disconnection, -2 - error in input: ");
    wrefresh (create_group_win);
    
    int result;
    wscanw (create_group_win, "%d", &result);
    /*----------------------------*/
    
    delwin (create_group_win);
    
    return result;
}
// quitting from a group
int quit_group() {
    WINDOW *quit_group_window = newwin (LINES, COLS, 0, 0);
    
    /*----------------------------*/
    wprintw (quit_group_window, "0 - successfully, -1 - disconnection, -2 - error in input: ");
    wrefresh (quit_group_window);
    
    int result;
    wscanw (quit_group_window, "%d", &result);
    /*----------------------------*/
    
    delwin (quit_group_window);
    
    return result;
}

// function for a thread of sending requests
void* send_request() {
    WINDOW *send_request_window = newwin (LINES / 4, COLS, LINES * 3 / 4 + 1, 0);
    
    
    
    delwin (send_request_window);
}
// function for a thread of receiving responses
void* recv_response() {
    WINDOW *recv_response_window = newwin (LINES * 3 / 4, COLS, 0, 0);

    message_t *message = (message_t *) malloc (sizeof (message_t));
    message->buffer = (char *) malloc (sizeof (char) * LENGTH_RESPONSE_MESSAGE);
    
    int result = 0;
    
    while (1) {
        int recv_status = recv (sock, message, sizeof (message), 0);
        if (recv_status > 0) {
            if (message->type == TYPE_CHAT_RECEIVE) {
                wprintw (recv_response_window, "\r%s\n", message->buffer);
            }
        } else if (recv_status == -1) {
            delwin (recv_response_window);
            result = ERROR_CONNECTION;
            free (message->buffer);
            free (message);
            return &result;
        } else {
            break;
        }
    }
    
    free (message->buffer);
    free (message);
    
    delwin (recv_response_window);
    return &result;
}

// sending a message
void send_message() {
    
}
// sending a file
void send_file() {
    
}
// changing a message
void change_message() {
    
}
// deleting a message
void delete_message() {
    
}

// inviting a new member of a group
void invite_member() {
    
}

// choosing an object of an action
void choose() {
    
}

// calling help for main menu
void help_main_menu() {
    WINDOW *help_main_menu_window = newwin (LINES, COLS, 0, 0);
    
    wprintw (help_main_menu_window, "\t1 - connect to the main chat\n");
    wprintw (help_main_menu_window, "\t2 - connect to a group chat\n");
    wprintw (help_main_menu_window, "\t3 - create a new chat\n");
    wprintw (help_main_menu_window, "\t4 - quit a group chat\n");
    wprintw (help_main_menu_window, "\t/exit - exit from the program\n");
    wprintw (help_main_menu_window, "Press any key to return back...");
    
    wrefresh (help_main_menu_window);
    noecho();
    getch();
    echo();
    
    delwin (help_main_menu_window);
}
// calling help for a main chat
void help_main_chat() {
    
}
// calling help for a group chat
void help_group_chat() {
    
}
// calling help for sending messages
void help_send_message() {
    
}
// calling help for sending files
void help_send_file() {
    
}
// calling help for changing messages
void help_change_message() {
    
}
// calling help for deleting messages
void help_delete_message() {
    
}
// calling help for inviting of new members of a group
void help_invite_member() {
    
}
// calling help for choosing an object of an action
void help_choose() {
    
}