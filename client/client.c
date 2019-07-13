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
    int choice = -1;
    scanf ("%d", &choice);
    getchar ();
    
    return choice;
}
// log in
int login() {
    printf ("1 - successful, 0 - cancel, -1 - unsuccessful: ");
    int choice;
    scanf ("%d", &choice);
    getchar ();
    
    return choice;
}
// registration
int registration() {
    printf ("1 - successful, 0 - cancel, -1 - unsuccessful: ");
    int choice;
    scanf ("%d", &choice);
    getchar ();
    
    return choice;
}

// main menu
void main_menu() {
    printf ("Welcome to ChatPrototype!\n");
    printf ("To find out which command is referred to which action, enter /help\n");
    printf (MSG_ENTER_COMMAND);
    
    while (1) {
        if (fgets (command, LENGTH_COMMAND, stdin) == NULL) {
            printf (MSG_ERROR);
            break;
        }
        
        trim_string(command);
        
        if (strcmp (command, "/help") == 0)
            help_main_menu();
        else if (strcmp (command, "/exit") == 0)
            break;
        else {
            if (strcmp (command, "1") == 0)
                main_chat();
            else if (strcmp (command, "2") == 0)
                group_chat();
            else if (strcmp (command, "3") == 0)
                create_group();
            else if (strcmp (command, "4") == 0)
                quit_group();
            else
                printf (MSG_ERROR_COMMAND);
        }
        
        printf (MSG_ENTER_COMMAND);
    }
}

// connecting to the main chat
void main_chat() {
    
}
// connecting to a group chat
void group_chat() {
    
}
// creating a new group
void create_group() {
    
}
// quitting from a group
void quit_group() {
    
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
    printf ("\t1 - connect to the main chat\n");
    printf ("\t2 - connect to a group chat\n");
    printf ("\t3 - create a new chat\n");
    printf ("\t4 - quit a group chat\n");
    printf ("\t/exit - exit from the program\n");
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