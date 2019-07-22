#include "server.h"

int server_sock;
struct sockaddr_in server_addr;
int threads_number;
client_list *now;
int exit_flag;
xmlDocPtr doc;

// handler for entering commands
void command_handler () {
    threads_number++;
    
    char command[LENGTH_COMMAND] = {};
    
    while (exit_flag != 1) {
        if (fgets (command, LENGTH_COMMAND, stdin) == NULL) {
            exit_flag = 1;
            break;
        }
        
        trim_string (command);
        if (strcmp (command, "/exit") == 0) {
            exit_flag = 1;
            break;
        }
    }
    
    threads_number--;
}
// handler for connecting of clients
void client_connecting_handler () {
    int client_sock;
    int server_addr_len = sizeof (server_addr);
    while (1) {
        // get a client socket
        if ((client_sock = accept (server_sock, (struct sockaddr*) &server_addr, (socklen_t*) &server_addr_len)) == -1) {
            return (EXIT_FAILURE);
        }
        
        printf ("%d\n", client_sock);
        
        // Append linked list for clients
/*
        client_list *c = new_node (client_sock);
        c->prev = now;
        now->next = c;
        now = c;
*/
        
        // run a thread for processing of client requests
        pthread_t id;
        if (pthread_create (&id, NULL, (void *) client_requests_handler, (void *) &client_sock) != 0) {
            return (EXIT_FAILURE);
        }
    }
}
// handler for processing of requests
void client_requests_handler (void *client_sock) {
    threads_number++;
    client_list *np = (client_list *) client_sock;
    message_t message, response;
    
    // Conversation
    while (exit_flag != 1) {
        memset ((char *) &message, '\0', sizeof (message));
        memset ((char *) &response, '\0', sizeof (response));
        
        if (recv (np->socket_id, (void *) &message, sizeof (message), 0) == -1) {
            printf ("\r%s", MSG_ERROR_THREADS);
            exit_flag = 1;
            break;
        }
        
        switch (message.type) {
            case (0) :
                response = login_request (message.buffer);
                break;
            case (1) :
                response = register_request (message.buffer);
                break;
/*
            case (2) :
                main_chat_request (message.buffer);
                break;
            case (3) :
                group_chat_request (message.buffer);
                break;
            case (4) :
                send_message_request (message.buffer);
                break;
            case (5) :
                send_message_request (message.buffer);
                break;
            case (6) :
                send_file_request (message.buffer);
                break;
            case (7) :
                change_message_request (message.buffer);
                break;
            case (8) :
                create_group_request (message.buffer);
                break;
            case (9) :
                quit_group_request (message.buffer);
*/
        }
        
        send (np->socket_id, (void *) &response, sizeof (response), 0);
    }

    // Remove Node
    close(np->socket_id);
    if (np == now) { // remove an edge node
        now = np->prev;
        now->next = NULL;
    } else { // remove a middle node
        np->prev->next = np->next;
        np->next->prev = np->prev;
    }
    free(np);
    
    threads_number--;
}

message_t login_request (char *message) {
    message_t response;
    memset (response.buffer, '\0', LENGTH_BUFFER);
    
    char nickname[LENGTH_NICKNAME];
    char password[LENGTH_PASSWORD];
    
    memset (nickname, '\0', LENGTH_NICKNAME);
    memset (password, '\0', LENGTH_PASSWORD);
    
    strncpy (nickname, message, LENGTH_NICKNAME);
    strncpy (password, message + (LENGTH_NICKNAME + 1), LENGTH_PASSWORD);
    
    
    
    while (doc != NULL) {}
    doc = xmlParseFile (DATABASE);
    xmlNodePtr cur = xmlDocGetRootElement (doc);
    
    cur = cur->children;
    while (xmlStrcmp (cur->name, (xmlChar *) "users") != 0)
        cur = cur->next;
    
    xmlChar *nick;
    xmlChar *pass;
    cur = cur->children;
    while (cur != NULL) {
        nick = xmlGetProp (cur, "nickname");
        pass = xmlGetProp (cur, "password");
        if (xmlStrcmp (nick, (xmlChar *) nickname) == 0 && xmlStrcmp (pass, (xmlChar *) password) != 0) {
            response.type = 1;  // not good
            strncpy (response.buffer, "Not OK", strlen ("Not OK"));
            break;
        } else if (xmlStrcmp (nick, (xmlChar *) nickname) == 0 && xmlStrcmp (pass, (xmlChar *) password) == 0) {
            response.type = 0;   // good
            strncpy (response.buffer, "OK", strlen ("OK"));
            break;
        }
    }
    
    
    
    xmlFreeDoc(doc);
    
    printf ("%s\n", nickname);
    printf ("%s\n", password);
    
    return response;   // not found
}
message_t register_request (char *message) {
    message_t response;
    memset (response.buffer, '\0', LENGTH_BUFFER);
    
    char nickname[LENGTH_NICKNAME];
    char password[LENGTH_PASSWORD];
    
    memset (nickname, '\0', LENGTH_NICKNAME);
    memset (password, '\0', LENGTH_PASSWORD);
    
    strncpy (nickname, message, LENGTH_NICKNAME);
    strncpy (password, message + (LENGTH_NICKNAME + 1), LENGTH_PASSWORD);
    
    
    while (doc != NULL) {}
    doc = xmlParseFile (DATABASE);
    xmlNodePtr cur = xmlDocGetRootElement (doc);
    
    cur = cur->children;
    while (xmlStrcmp (cur->name, (xmlChar *) "users") != 0)
        cur = cur->next;
    
    printf ("%s\n", (char *) cur->name);
    
    cur = cur->children;
    xmlNodePtr newnode = xmlNewTextChild (cur, NULL, "user", NULL);
    xmlNewProp (newnode, "nickname", nickname);
    xmlNewProp (newnode, "password", password);
    
    response.type = 0;   // good
    strncpy (response.buffer, "OK", strlen ("OK"));
    
    
    
    
    xmlSaveFormatFile (DATABASE, doc, 1);
    xmlFreeDoc(doc);

    printf ("%s\n", nickname);
    printf ("%s\n", password);
    
    return response;   // not found
}



// processing a request for connecting to the main chat
//void main_chat_request (int user_id) {
    /*
     * 1. open XML-file
     * 2. find a tag <main_chat>
     * 3. send to a client the data between <main_chat> and </main_chat> by string after string
     * 4. send to a client the result of the procedure
     * 5. add client id to the list of online-in-chat users
     */
//}
// processing a request for connecting to a group chat
//void group_chat_request (int user_id) {
    
//}
// processing a request for sending a message
//void send_message_request (char *message) {
    /*
     * 1. open XML-file
     * 2. find a tag <main_chat>
     * 3. add the message into the file before the tag </main_chat>
     * 4. send to a sender the result of the procedure
     * 5. send to other clients the message
     */
//}
// processing a request for sending a file
//void send_file_request () {
    
//}
/*
// processing a request for changing a message
void change_message_request (int);
// processing a request for creating a group
void create_group_request ();
// processing a request for inviting a new member to a group
void invite_group_request ();
// processing a request for quitting from a group
void quit_group_request (int);

// writing a message to log-file
void write_log (const char *message) {
    
}
*/
//
client_list *new_node (int sockfd) {
    client_list *np = (client_list *) malloc ( sizeof (client_list) );
    np->socket_id = sockfd;
    np->prev = NULL;
    np->next = NULL;
    //strncpy(np->name, "NULL", 4);
    return np;
}

void str_overwrite_stdout() {
    printf("\r%s", "> ");
    fflush(stdout);
}
void trim_string (char * string) {
    int i = strlen (string) - 1;
    if (string[i] == '\n') string [i] = '\0';
}