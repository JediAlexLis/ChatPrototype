#include <unicode/umachine.h>

#include "client.h"

int sock;
char command[LENGTH_CHAT_MESSAGE];
char username[LENGTH_NICKNAME];
chat_message_t *chat_messages_list;
chat_message_t *last;
int exit_from_chat;
int client_message_count;

pthread_t send_request_thread;
pthread_t recv_response_thread;

// trimming \n
void trim_string (char * string) {
    int i = strlen (string) - 1;
    if (string[i] == '\n') string [i] = '\0';
}

// connecting to the server
int connect_server() {
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
}
// enter an account
int enter_account() {
    WINDOW *enter_account_window = newwin (LINES, COLS, 0, 0);
    
    // request informing the server that a user is going to exit the program
    message_t *exit = (message_t *) malloc (sizeof (message_t));
    exit->type = 0;
    
    wprintw (enter_account_window, MSG_ENTER_WHEN_CONNECTED);
    refresh ();
    
    // choice of login or registration
    int login_status, registration_status;
    while (1) {
        if (wgetnstr (enter_account_window, command, LENGTH_COMMAND) == ERR) { // error with input
            delwin (enter_account_window);
            free (exit);
            return ERROR_INPUT;
        }

        trim_string (command);
        
        if (strcmp (command, "0") == 0) {   // to exit the program
            if (send (sock, (void *) exit, sizeof (message_t), 0) == -1) {
                delwin (enter_account_window);
                free (exit);
                return ERROR_CONNECTION;
            }
            delwin (enter_account_window);
            return 0;
        }
        else if (strcmp (command, "1") == 0) {  // to log in
            if ((login_status = login()) == ERROR_INPUT) {   // error in input
                delwin (enter_account_window);
                free (exit);
                return ERROR_INPUT;
            } else if (login_status == ERROR_CONNECTION) {    // error in connection
                delwin (enter_account_window);
                free (exit);
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
                free (exit);
                return ERROR_INPUT;
            } else if (registration_status == ERROR_CONNECTION) {             // error in connection
                delwin (enter_account_window);
                free (exit);
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
    free (exit);
    return 1;
}
// log in
// 1 - successfully, 0 - cancel, -1 - disconnection, -2 - error in input
int login() {
    WINDOW *login_window = newwin (LINES, COLS, 0, 0);
    
    char *nickname = (char *) malloc (LENGTH_NICKNAME * sizeof (char));
    char *password = (char *) malloc (LENGTH_PASSWORD * sizeof (char));
    message_t *message = (message_t *) malloc (sizeof (message_t));
    
    while (1) {
        memset (nickname, '\0', LENGTH_NICKNAME);
        memset (password, '\0', LENGTH_PASSWORD);
        
        wprintw (login_window, "Enter /cancel to return back.\n\n");
        wprintw (login_window, "Enter a nickname: ");
        wrefresh (login_window);
        wgetnstr (login_window, nickname, LENGTH_NICKNAME);
        if (strcmp (nickname, "/cancel") == 0) {
            free (nickname);
            free (password);
            free (message);
            delwin (login_window);
            return 0;
        }

        wprintw (login_window, "Enter a password: ");
        wrefresh (login_window);
        wgetnstr (login_window, password, LENGTH_PASSWORD);
        if (strcmp (password, "/cancel") == 0) {
            free (nickname);
            free (password);
            free (message);
            delwin (login_window);
            return 0;
        }

        memset ((char *) message, '\0', sizeof (message_t));

        message->type = 1;
        strncpy (message->buffer, nickname, LENGTH_NICKNAME);
        strncpy (message->buffer + (LENGTH_NICKNAME + 1), password, LENGTH_PASSWORD);

        // send a request
        if (send (sock, (void *) message, sizeof (message_t), 0) == -1)
            return ERROR_CONNECTION;

        memset ((char *) message, '\0', sizeof (message_t));
        // receive a response
        if (recv (sock, (void *) message, sizeof (message_t), 0) == -1)
            return ERROR_CONNECTION;
        // check if type is equal to 0, that is logging was done successfully
        if (!message->type)
            break;
        
        // print a message in case of unsuccessful logging
        wprintw (login_window, "%s\n", message->buffer);
        wrefresh (login_window);
    }
    
    strncpy (username, nickname, strlen (nickname));
    free (nickname);
    free (password);
    free (message);
    delwin (login_window);
    
    return 1;
}
// registration
// 1 - successfully, 0 - cancel, -1 - disconnection, -2 - error in input
int registration() {
    WINDOW *registration_window = newwin (LINES, COLS, 0, 0);
    
    char *nickname = (char *) malloc (LENGTH_NICKNAME * sizeof (char));
    char *password = (char *) malloc (LENGTH_PASSWORD * sizeof (char));
    message_t *message = (message_t *) malloc (sizeof (message_t));
    
    while (1) {
        memset (nickname, '\0', LENGTH_NICKNAME);
        memset (password, '\0', LENGTH_PASSWORD);
        
	wprintw (registration_window, "Enter /cancel to return back.\n\n");
        wprintw (registration_window, "Enter a nickname: ");
        wrefresh (registration_window);
        wgetnstr (registration_window, nickname, LENGTH_NICKNAME);
        if (strcmp (nickname, "/cancel") == 0) {
            free (nickname);
            free (password);
            free (message);
            delwin (registration_window);
            return 0;
        }

        wprintw (registration_window, "Enter a password: ");
        wrefresh (registration_window);
        wgetnstr (registration_window, password, LENGTH_PASSWORD);
        if (strcmp (password, "/cancel") == 0) {
            free (nickname);
            free (password);
            free (message);
            delwin (registration_window);
            return 0;
        }

        memset ((char *) message, '\0', sizeof (message_t));

        message->type = 2;
        strncpy (message->buffer, nickname, LENGTH_NICKNAME);
        strncpy (message->buffer + (LENGTH_NICKNAME + 1), password, LENGTH_PASSWORD);

	// send a request
        if (send (sock, (void *) message, sizeof (message_t), 0) == -1)
            return ERROR_CONNECTION;
        memset ((char *) message, '\0', sizeof (message_t));
        // receive a response
        if (recv (sock, (void *) message, sizeof (message_t), 0) == -1)
            return ERROR_CONNECTION;
        
	// check if type is equal to 0, that is registration was done successfully
        if (!message->type)
            break;

	// print a message in case of unsuccessful registration
	wprintw (registration_window, "%s\n", message->buffer);
        wrefresh (registration_window);
    }

    free (nickname);
    free (password);
    free (message);
    delwin (registration_window);
    
    return 1;
}

// main menu
int main_menu() {  
    WINDOW *main_menu_window = newwin (LINES, COLS, 0, 0);
    
    // request informing the server that a user is going to exit the program
    message_t *exit = (message_t *) malloc (sizeof (message_t));
    exit->type = 0;
    
    wprintw (main_menu_window, "Welcome to ChatPrototype!\n");
    wprintw (main_menu_window, "To find out which command is referred to which action, enter /help\n");
    wprintw (main_menu_window, MSG_ENTER_COMMAND);
    wrefresh (main_menu_window);
    
    int status;
    while (1) {
        if (wgetnstr (main_menu_window, command, LENGTH_COMMAND) == ERR) {  // error in input
            delwin (main_menu_window);
            free (exit);
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
        else if (strcmp (command, "/exit") == 0) {   // exit the program
            if (send (sock, (void *) exit, sizeof (message_t), 0) == -1) {
                delwin (main_menu_window);
                free (exit);
                return ERROR_CONNECTION;
            }
            break;
        }
        else {
            if (strcmp (command, "1") == 0) {   // enter the main chat
                if ((status = main_chat()) == ERROR_THREAD) {   // error in threads
                    delwin (main_menu_window);
                    free (exit);
                    return ERROR_THREAD;
                } else if (status == ERROR_INPUT) {         // error in input
                    delwin (main_menu_window);
                    free (exit);
                    return ERROR_INPUT;
                } else if (status == ERROR_CONNECTION) {    // error in connection
                    delwin (main_menu_window);
                    free (exit);
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
                    free (exit);
                    return ERROR_THREAD;
                } else if (status == ERROR_INPUT) {         // error in input
                    delwin (main_menu_window);
                    free (exit);
                    return ERROR_INPUT;
                } else if (status == ERROR_CONNECTION) {    // error in connection
                    delwin (main_menu_window);
                    free (exit);
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
                    free (exit);
                    return ERROR_INPUT;
                } else if (status == ERROR_CONNECTION) {        // error in connection
                    delwin (main_menu_window);
                    free (exit);
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
                    free (exit);
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
    free (exit);
    return 0;
}

// connecting to the main chat
int main_chat() {
    WINDOW *main_chat_window = newwin (LINES, COLS, 0, 0);
    
    wprintw (main_chat_window, "*** To find out which command is referred to which action, enter /help ***\n");
    
    exit_from_chat = 0;
    
    // chat message structure
    struct chat_message_s {
        char name[LENGTH_NICKNAME];
        char message[LENGTH_CHAT_MESSAGE];
        char id[6];
    } chat_message;
    
    // sending a request
    message_t request;
    memset ((char *) &request, '\0', sizeof (message_t));
    request.type = 3;
    send (sock, (void *) &request, sizeof (message_t), 0);
    
    // creating a file
    int result = 1;
    //FILE *chat = fopen ("chat.txt", "w");
    
    // receiving messages from the main chat
    chat_message_t *node;
    while (1) {
        recv (sock, (void *) &chat_message, sizeof (chat_message), 0);
        if (strcmp (chat_message.message, "/all") == 0) {
            //wprintw (main_chat_window, "All!\n");
            break;
        }
        
        if (strcmp (chat_message.name, username) == 0) {
            wprintw (main_chat_window, "%s\n", chat_message.message);
            
            // adding a new element to the list
            node = (chat_message_t *) malloc (sizeof (chat_message_t));
            node->number = client_message_count++;
            strncpy (node->id, chat_message.id, 6);
            strncpy (node->message, chat_message.message, strlen (chat_message.message));
            strncpy (node->sender, chat_message.name, strlen (chat_message.name));
            node->next = node->prev = NULL;
            add_new_node (node);
        }
        else
            wprintw (main_chat_window, "%s: %s\n", chat_message.name, chat_message.message);
        
        //fprintf (chat, "%s: %s\n", chat_message.name, chat_message.message);
    }
    wrefresh (main_chat_window);
    
    // closing a file descriptor
    //fclose (chat);
    
    
    if (pthread_create(&send_request_thread, NULL, (void *) send_request, (void *) main_chat_window) != 0) {
        return ERROR_THREAD;
    }

    if (pthread_create(&recv_response_thread, NULL, (void *) recv_response, (void *) main_chat_window) != 0) {
        return ERROR_THREAD;
    }
    
    //wprintw (main_chat_window, "Before exit...\n");
    
    while (exit_from_chat != 1) {}
    
    pthread_cancel (send_request_thread);
    pthread_cancel (recv_response_thread);
    
    //wprintw (main_chat_window, "Before exit...\n");
    
    // deleting the list
    while (last != NULL) {
        node = last;
        
        wprintw (main_chat_window, "***node = last;\n");
        wrefresh (main_chat_window);
        
        last = last->prev;
        
        wprintw (main_chat_window, "***last = last->prev;\n");
        wrefresh (main_chat_window);
        
        node->prev = NULL;
        
        wprintw (main_chat_window, "***node->prev = NULL;\n");
        wrefresh (main_chat_window);
        
        free (node);
        
        wprintw (main_chat_window, "***free (node)\n");
        wrefresh (main_chat_window);
        
        if (last != NULL) {
            last->next = NULL;
            
            wprintw (main_chat_window, "***last->next = NULL;\n");
            wrefresh (main_chat_window);
        }
    }
    chat_messages_list = NULL;
    last = NULL;
    client_message_count = 0;
    
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
void send_request(void *window) {
    pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, NULL);
    
    message_t request;
    
    // chat message structure
    struct chat_message_s {
        char name[LENGTH_NICKNAME];
        char message[LENGTH_CHAT_MESSAGE];
        char id[6];
    } chat_message;
    
    while (exit_from_chat != 1) {
        memset ((char *) &request, '\0', sizeof (request));
        memset (command, '\0', LENGTH_CHAT_MESSAGE);
        
        wgetnstr ((WINDOW *) window, command, LENGTH_CHAT_MESSAGE);
        
        if (strcmp (command, "/change") == 0) {
            pthread_cancel (recv_response_thread);
            change_message ();
            delwin ((WINDOW *) window);
            
            window = (void *) newwin (LINES, COLS, 0, 0);
            
            wprintw ((WINDOW *) window, "*** To find out which command is referred to which action, enter /help ***\n");
            
            // sending a request
            message_t request;
            memset ((char *) &request, '\0', sizeof (message_t));
            request.type = 3;
            send (sock, (void *) &request, sizeof (message_t), 0);

            // receiving messages from the main chat
            while (1) {
                recv (sock, (void *) &chat_message, sizeof (chat_message), 0);
                if (strcmp (chat_message.message, "/all") == 0) {
                    break;
                }

                if (strcmp (chat_message.name, username) == 0)
                    wprintw (window, "%s\n", chat_message.message);
                else
                    wprintw (window, "%s: %s\n", chat_message.name, chat_message.message);

            }
            wrefresh ((WINDOW *) window);
            pthread_create(&recv_response_thread, NULL, (void *) recv_response, window);
        } else if (strcmp (command, "/help") == 0) {
            pthread_cancel (recv_response_thread);
            help_main_chat ();
            delwin ((WINDOW *) window);
            
            window = (void *) newwin (LINES, COLS, 0, 0);
            
            wprintw ((WINDOW *) window, "*** To find out which command is referred to which action, enter /help ***\n");
            
            // sending a request
            message_t request;
            memset ((char *) &request, '\0', sizeof (message_t));
            request.type = 3;
            send (sock, (void *) &request, sizeof (message_t), 0);

            // receiving messages from the main chat
            while (1) {
                recv (sock, (void *) &chat_message, sizeof (chat_message), 0);
                if (strcmp (chat_message.message, "/all") == 0) {
                    break;
                }

                if (strcmp (chat_message.name, username) == 0)
                    wprintw (window, "%s\n", chat_message.message);
                else
                    wprintw (window, "%s: %s\n", chat_message.name, chat_message.message);

            }
            wrefresh ((WINDOW *) window);
            pthread_create(&recv_response_thread, NULL, (void *) recv_response, window);
        } else if (strcmp (command, "/exit") == 0) {   // exit the program
            request.type = REQUEST_EXIT_MAIN_CHAT;
            send (sock, (void *) &request, sizeof (message_t), 0);
            //recv (sock, (void *) &response, sizeof (message_t), 0);
            exit_from_chat = 1;
            //continue;
        } else {
            request.type = REQUEST_SEND_MESSAGE;
            strncpy (request.buffer, username, LENGTH_NICKNAME);
            strncpy (request.buffer + (LENGTH_NICKNAME + 1), command, LENGTH_CHAT_MESSAGE);
            send (sock, (void *) &request, sizeof (message_t), 0);
            
            
            //recv (sock, (void *) &response, sizeof (message_t), 0);
        }
    }
}
// function for a thread of receiving responses
void recv_response(void *window) {
    pthread_setcancelstate (PTHREAD_CANCEL_ENABLE, NULL);
    
    chat_message_t *node;
    
    // chat message structure
    struct chat_message_s {
        char name[LENGTH_NICKNAME];
        char message[LENGTH_CHAT_MESSAGE];
        char id[6];
    } chat_message;
    
    //message_t *message = (message_t *) malloc (sizeof (message_t));
    //message->buffer = (char *) malloc (sizeof (char) * LENGTH_RESPONSE_MESSAGE);
    
    message_t response;
    char user[LENGTH_NICKNAME];
    char message[LENGTH_CHAT_MESSAGE];
    
    while (1) {
        memset (user, '\0', LENGTH_NICKNAME);
        memset (message, '\0', LENGTH_CHAT_MESSAGE);
        recv (sock, (void *) &response, sizeof (message_t), 0);
        if (response.type == RESPONSE_CHAT_MESSAGE) {
            strncpy (user, response.buffer, LENGTH_NICKNAME);
            strncpy (message, response.buffer + (LENGTH_NICKNAME + 1), LENGTH_CHAT_MESSAGE);
            wprintw ((WINDOW *) window, "%s: %s\n", user, message);
            wrefresh ((WINDOW *) window);
        } else if (response.type == RESPONSE_UPDATE_CHAT) {
            delwin ((WINDOW *) window);
            
            window = (void *) newwin (LINES, COLS, 0, 0);
            
            wprintw ((WINDOW *) window, "*** To find out which command is referred to which action, enter /help ***\n");
            
            // sending a request
            message_t request;
            memset ((char *) &request, '\0', sizeof (message_t));
            request.type = 3;
            send (sock, (void *) &request, sizeof (message_t), 0);

            // receiving messages from the main chat
            while (1) {
                recv (sock, (void *) &chat_message, sizeof (chat_message), 0);
                if (strcmp (chat_message.message, "/all") == 0) {
                    break;
                }

                if (strcmp (chat_message.name, username) == 0)
                    wprintw (window, "%s\n", chat_message.message);
                else
                    wprintw (window, "%s: %s\n", chat_message.name, chat_message.message);

            }
            wrefresh ((WINDOW *) window);
        } else if (response.type == RESPONSE_SENT_MESSAGE) {
            strncpy ((char *) &chat_message, response.buffer, strlen (response.buffer));
            
            // adding a new element to the list
            node = (chat_message_t *) malloc (sizeof (chat_message_t));
            node->number = client_message_count++;
            strncpy (node->id, chat_message.id, 6);
            strncpy (node->message, chat_message.message, strlen (chat_message.message));
            strncpy (node->sender, chat_message.name, strlen (chat_message.name));
            node->next = node->prev = NULL;
            add_new_node (node);
        } else {
            //wprintw ((WINDOW *) window, "\r*** Message is sent! ***\n");
            //wrefresh ((WINDOW *) window);
        }
    }
    
}

// sending a message
void send_message() {
    
}
// sending a file
void send_file() {
    
}
// changing a message
void change_message() {
    WINDOW *change_message_window = newwin (LINES, COLS, 0, 0);
    
    chat_message_t *ptr = chat_messages_list;
    int choice;
    int max = 0;
    
    char new_message[LENGTH_CHAT_MESSAGE];
    
    message_t request, response;
    memset ((char *) &request, '\0', sizeof (message_t));
    memset ((char *) &response, '\0', sizeof (message_t));
    memset (new_message, '\0', LENGTH_CHAT_MESSAGE);
    
    if (ptr == NULL) {
        wprintw (change_message_window, "You haven't written any your message yet\nPress any key to return... ");
        wrefresh (change_message_window);
        noecho();
        getch();
        echo();
    } else {
        while (ptr != NULL) {
            wprintw (change_message_window, "%d. %s\n", ptr->number, ptr->message);
            max = ptr->number;
            ptr = ptr->next;
        }
        wprintw (change_message_window, "\nEnter a number of a message you want to change: ");
        wrefresh (change_message_window);
        wscanw (change_message_window, "%d", &choice);
        
        wprintw (change_message_window, "Enter new message: ");
        wgetnstr (change_message_window, new_message, LENGTH_CHAT_MESSAGE);
        
        ptr = chat_messages_list;
        while (ptr->number != choice) {
            ptr = ptr->next;
        }
        
        request.type = 5;
        strncpy (request.buffer, ptr->id, strlen (ptr->id));
        strncpy (request.buffer + (strlen (ptr->id) + 1), new_message, strlen (new_message));
        send (sock, (void *) &request, sizeof (message_t), 0);
        recv (sock, (void *) &response, sizeof (message_t), 0);
        
        strncpy (ptr->message, new_message, strlen (new_message));
    }
    
    delwin (change_message_window);
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
    WINDOW *help_main_chat_window = newwin (LINES, COLS, 0, 0);
    
    wprintw (help_main_chat_window, "\tTo send a message, write it and press Enter\n");
    wprintw (help_main_chat_window, "\tTo exit the chat, write /exit and press Enter\n");
    wprintw (help_main_chat_window, "Press any key to return back...");
    
    wrefresh (help_main_chat_window);
    noecho();
    getch();
    echo();
    
    delwin (help_main_chat_window);
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

void add_new_node (chat_message_t *node) {
    if (chat_messages_list == NULL) {
        chat_messages_list = node;
        last = node;
    } else {
        node->prev = last;
        last->next = node;
        last = node;
    }
}