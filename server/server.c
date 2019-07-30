#include <stdbool.h>

#include "server.h"

int server_sock;
struct sockaddr_in client_info;
int requests_number;
client_list *last_client;
client_list *first_client;
int exit_flag;
xmlDocPtr doc;

// handler for entering commands
void command_handler () {
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
}
// handler for connecting of clients
void client_connecting_handler () {
    int client_sock;
    struct sockaddr_in client_info;
    int client_info_len = sizeof (client_info);
    
    while (1) {
        // get a client socket
        if ((client_sock = accept (server_sock, (struct sockaddr*) &client_info, (socklen_t*) &client_info_len)) == -1) {
            printf ("%s", MSG_ERROR_CONNECTION);
            return;
        }
        printf ("Client %s is accepted!\n", inet_ntoa(client_info.sin_addr));
        
        // Append linked list for clients
        client_list *c = new_node (client_sock, inet_ntoa(client_info.sin_addr));
        if (last_client != NULL) {
            c->prev = last_client;
            last_client->next = c;
        } else
            first_client = c;
        last_client = c;
        
        printf ("Creating of a thread for %s...\n", c->ip);
        // run a thread for processing of client requests
        pthread_t id;
        if (pthread_create (&id, NULL, (void *) client_requests_handler, (void *) c) != 0) {
            printf ("%s", MSG_ERROR_THREADS);
            return;
        }
        printf ("Thread for %s is created!\n", c->ip);
    }
}
// handler for processing of requests
void client_requests_handler (void *client) {
    client_list *np = (client_list *) client;
    message_t *request, *response;
    request = (message_t *) malloc (sizeof (message_t));
    
    // Conversation
    while (exit_flag != 1) {
        memset ((char *) request, '\0', sizeof (message_t));

        if (recv (np->socket_fd, (void *) request, sizeof (message_t), 0) == -1) {
            printf ("\r%s", MSG_ERROR_CONNECTION);
            exit_flag = 1;
            continue;
        }
        requests_number++;
        
        printf ("Request from %s/%s is received!\n", np->name, np->ip);

        if (!request->type) {    // type == 0 - user wants to disconnect from the server and exit the program
            requests_number--;
            break;
        }
        // recognizing of a request
        switch (request->type) {
            case (1) : // to log in
                printf ("\tIt's a request to login\n");
                if ((response = login_request (request->buffer, np)) == NULL) {
                    requests_number--;
                    exit_flag = 1;
                    continue;
                }
                break;
            case (2) :  // to register
                printf ("\tIt's a request to register\n");
                if ((response = register_request (request->buffer)) == NULL) {
                    requests_number--;
                    exit_flag = 1;
                    continue;
                }
                break;
            case (3) :  // to join the main chat
                printf ("\tIt's a request to join the main chat\n");
                if ((response = main_chat_request (np)) == NULL) {
                    requests_number--;
                    exit_flag = 1;
                    continue;
                }
                break;
            case (4) :  // to send a message into the main chat
                printf ("\tIt's a request to send a message into the main chat\n");
                if ((response = send_message_request (np, request->buffer)) == NULL) {
                    requests_number--;
                    exit_flag = 1;
                    continue;
                }
                break;
            case (5) :  // to change a message from the main chat
                printf ("\tIt's a request to change a message from the main chat\n");
                if ((response = change_message_request (np, request->buffer)) == NULL) {
                    requests_number--;
                    exit_flag = 1;
                    continue;
                }
                break;
/*
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
        printf ("Sending of a response to %s/%s...\n", np->name, np->ip);
        if (send (np->socket_fd, (void *) response, sizeof (message_t), 0) == -1) {
            printf ("\r%s", MSG_ERROR_CONNECTION);
            exit_flag = 1;
        } else
            printf ("Response to %s/%s is sent!\n", np->name, np->ip);
        requests_number--;
    }
    
    // Remove Node
    close(np->socket_fd);
    printf ("%s/%s is disconnected from the server\n", np->name, np->ip);
    if (np == last_client) {        // remove a last node
        if (np->prev != NULL) { // check if it's not the first node
            last_client = np->prev;
            last_client->next = NULL;
        }
    } else {                        // otherwise
        if (np->prev != NULL)   // check if it's not the first node
            np->prev->next = np->next;
        np->next->prev = np->prev;
    }

    free(np);
}

// processor of the request to log in
message_t* login_request (char *message, client_list *np) {
    // variable for response to a client
    message_t *response = (message_t *) malloc (sizeof (message_t));
    memset (response->buffer, '\0', LENGTH_BUFFER);
    
    // variables for nickname and password
    char *nickname = (char *) malloc (LENGTH_NICKNAME * sizeof (char));
    char *password = (char *) malloc (LENGTH_PASSWORD * sizeof (char));
    
    // variable which responds to whether the user with the specific nickname is found in the database
    uint8_t isFound = 0;
    
    memset (nickname, '\0', LENGTH_NICKNAME);
    memset (password, '\0', LENGTH_PASSWORD);
    
    strncpy (nickname, message, LENGTH_NICKNAME);
    strncpy (password, message + (LENGTH_NICKNAME + 1), LENGTH_PASSWORD);
    
    // parsing XML file
    while (doc != NULL) {}
    if ((doc = xmlParseFile (DATABASE)) == NULL) {
        printf ("%s", MSG_ERROR_WORK_XML);
        free (nickname);
        free (password);
        xmlFreeDoc(doc);
        return NULL;
    }
    xmlNodePtr cur = xmlDocGetRootElement (doc);
    
    // finding a <users> tag in XML
    cur = cur->children;
    while (xmlStrcmp (cur->name, (xmlChar *) "users") != 0)
        cur = cur->next;
    
    // finding a <user> node with specific attributes of a nickname and a password
    xmlChar *nick, *pass;
    cur = cur->children;
    while (cur != NULL) {
        if (xmlStrcmp (cur->name, (xmlChar *) "text") == 0) {
            cur = cur->next;
            continue;
        }
        nick = xmlGetProp (cur, "nickname");
        pass = xmlGetProp (cur, "password");
        if (nick == NULL || pass == NULL) {
            printf ("%s", MSG_ERROR_WORK_XML);
            free (nickname);
            free (password);
            xmlFreeDoc(doc);
            doc = NULL;
            return NULL;
        }
        if (xmlStrcmp (nick, (xmlChar *) nickname) == 0) {
            if (xmlStrcmp (pass, (xmlChar *) password) != 0) {  // if nickname is found, but password isn't correct
                response->type = 1;  // unsuccessful logging
                strncpy (response->buffer, "Username/password is not correct!", strlen ("Username/password is not correct!"));
                isFound = 1;
                break;
            } else {                                            // otherwise
                response->type = 0;   // successful logging
                strncpy (response->buffer, "OK", strlen ("OK"));
                strncpy (np->name, nickname, strlen (nickname));
                printf ("%s from %s is logged in\n", nickname, np->ip);
                isFound = 1;
                break;
            }
        }
        
        cur = cur->next;
    }

    if (isFound == 0) {
        response->type = 1;  // unsuccessful logging
        strncpy (response->buffer, "User with such nickname is not found!", strlen ("User with such nickname is not found!"));
    }
    
    free (nickname);
    free (password);
    xmlFreeDoc(doc);
    doc = NULL;
    
    return response;
}
// processor of the request to register
message_t* register_request (char *message) {
    // variable for response to a client
    message_t *response = (message_t *) malloc (sizeof (message_t));
    memset (response->buffer, '\0', LENGTH_BUFFER);
    
    // variables for nickname and password
    char *nickname = (char *) malloc (LENGTH_NICKNAME * sizeof (char));
    char *password = (char *) malloc (LENGTH_PASSWORD * sizeof (char));
    
    memset (nickname, '\0', LENGTH_NICKNAME);
    memset (password, '\0', LENGTH_PASSWORD);
    
    strncpy (nickname, message, LENGTH_NICKNAME);
    strncpy (password, message + (LENGTH_NICKNAME + 1), LENGTH_PASSWORD);
    
    // parsing XML file
    while (doc != NULL) {}
    if ((doc = xmlParseFile (DATABASE)) == NULL) {
        printf ("%s", MSG_ERROR_WORK_XML);
        xmlSaveFormatFile (DATABASE, doc, 1);
        xmlFreeDoc(doc);
        free (nickname);
        free (password);
        return NULL;
    }
    xmlNodePtr cur = xmlDocGetRootElement (doc);
    
    // finding a <users> tag in XML
    cur = cur->children;
    while (xmlStrcmp (cur->name, (xmlChar *) "users") != 0)
        cur = cur->next;
    
    // finding a <user> node with the same attributes of a nickname and a password
    xmlChar *nick, *pass;
    xmlNodePtr ptr = cur->children;
    while (ptr != NULL) {
        if (xmlStrcmp (ptr->name, (xmlChar *) "text") == 0) {
            ptr = ptr->next;
            continue;
        }
        nick = xmlGetProp (ptr, "nickname");
        pass = xmlGetProp (ptr, "password");
        if (nick == NULL || pass == NULL) {
            printf ("%s", MSG_ERROR_WORK_XML);
            free (nickname);
            free (password);
            xmlFreeDoc(doc);
            doc = NULL;
            return NULL;
        }
        if (xmlStrcmp (nick, (xmlChar *) nickname) == 0) {      // if the same nickname is found
            response->type = 1;  // unsuccessful logging
            strncpy (response->buffer, "User with such nickname already exists", strlen ("User with such nickname already exists"));
            break;
        }
        
        ptr = ptr->next;
    }
    
    // adding a new <user> node with the specific attributes
    xmlNodePtr newnode;
    if (ptr == NULL) {
        if ((newnode = xmlNewTextChild (cur, NULL, "user", NULL)) == NULL) {
            printf ("%s", MSG_ERROR_WORK_XML);
            xmlSaveFormatFile (DATABASE, doc, 1);
            xmlFreeDoc(doc);
            free (nickname);
            free (password);
            doc = NULL;
            return NULL;
        }
        if (xmlNewProp (newnode, "nickname", nickname) == NULL) {
            printf ("%s", MSG_ERROR_WORK_XML);
            xmlSaveFormatFile (DATABASE, doc, 1);
            xmlFreeDoc(doc);
            free (nickname);
            free (password);
            doc = NULL;
            return NULL;
        }
        if (xmlNewProp (newnode, "password", password) == NULL) {
            printf ("%s", MSG_ERROR_WORK_XML);
            xmlSaveFormatFile (DATABASE, doc, 1);
            xmlFreeDoc(doc);
            free (nickname);
            free (password);
            doc = NULL;
            return NULL;
        }
        if (xmlNewProp (newnode, "banned", "false") == NULL) {
            printf ("%s", MSG_ERROR_WORK_XML);
            xmlSaveFormatFile (DATABASE, doc, 1);
            xmlFreeDoc(doc);
            free (nickname);
            free (password);
            doc = NULL;
            return NULL;
        }

        response->type = 0;   // successful registration
        strncpy (response->buffer, "OK", strlen ("OK"));
        printf ("New user %s is registered\n", nickname);
    }
    
    xmlSaveFormatFile (DATABASE, doc, 1);
    xmlFreeDoc(doc);
    free (nickname);
    free (password);
    doc = NULL;

    return response;
}



// processing a request for connecting to the main chat
message_t* main_chat_request (client_list *np) {
    /*
     * 1. open XML-file
     * 2. find a tag <main_chat>
     * 3. send to a client the data between <main_chat> and </main_chat> by string after string
     * 4. send to a client the result of the procedure
     * 5. add client id to the list of online-in-chat users */
    
    // chat message structure
    struct chat_message_s {
        char name[LENGTH_NICKNAME];
        char text[LENGTH_CHAT_MESSAGE];
        char id[6];
    } chat_message;
    
    // parsing XML file
    while (doc != NULL) {}
    if ((doc = xmlParseFile (DATABASE)) == NULL) {
        printf ("%s", MSG_ERROR_WORK_XML);
        xmlSaveFormatFile (DATABASE, doc, 1);
        xmlFreeDoc(doc);
        return NULL;
    }
    xmlNodePtr cur = xmlDocGetRootElement (doc);
    
    printf ("\t\tFile is parsed!\n");
    
    // finding a <main_chat> tag in XML
    cur = cur->children;
    while (xmlStrcmp (cur->name, (xmlChar *) "main_chat") != 0)
        cur = cur->next;
    
    printf ("\t\tTag is found!\n");
    
    // finding a <message> node
    cur = cur->children;
    while (cur != NULL) {
        if (xmlStrcmp (cur->name, (xmlChar *) "text") == 0) {
            cur = cur->next;
            continue;
        }
        memset ((char *) &chat_message, '\0', sizeof (chat_message));
        strncpy (chat_message.name, (char *) xmlGetProp (cur, "name"), strlen ((char *) xmlGetProp (cur, "name")));
        strncpy (chat_message.text, (char *) xmlGetProp (cur, "text"), strlen ((char *) xmlGetProp (cur, "text")));
        strncpy (chat_message.id, ((char *) xmlGetProp (cur, "id")), strlen ((char *) xmlGetProp (cur, "id")));
        
        //printf ("%s: %s\n", (char *) xmlGetProp (cur, "name"), (char *) xmlGetProp (cur, "message"));
        //printf ("Length of name: %d\tLength of message: %d\n", strlen((char *) xmlGetProp (cur, "name")), strlen ((char *) xmlGetProp (cur, "message")));
        
/*
        if (strlen (chat_message.name) == 0 || strlen (chat_message.name) == 0) {
            printf ("%s", MSG_ERROR_WORK_XML);
            xmlFreeDoc(doc);
            remove ("tmp.txt");
            doc = NULL;
            return NULL;
        }
*/
        //size += write (tmp, (void *) chat_message, sizeof (chat_message));
        send (np->socket_fd, (void *) &chat_message, sizeof (chat_message), 0);
        printf ("\t\tA message (%s) from the chat is sent to %s/%s\n", chat_message.text, np->name, np->ip);
        
        cur = cur->next;
    }
    memset ((char *) &chat_message, '\0', sizeof (chat_message));
    strncpy (chat_message.text, "/all", strlen ("/all"));
    send (np->socket_fd, (void *) &chat_message, sizeof (chat_message), 0);
    
    printf ("\t\t/all is sent to %s/%s\n", np->name, np->ip);
    
    xmlFreeDoc (doc);
    doc = NULL;
    
    message_t *new_response = (message_t *) malloc (sizeof (message_t));
    new_response->type = 100;
    
    return new_response;
}

// processing a request for connecting to a group chat
//void group_chat_request (int user_id) {
    
//}
// processing a request for sending a message
message_t* send_message_request (client_list *np, char * buffer) {
    /*
     * 1. open XML-file
     * 2. find a tag <main_chat>
     * 3. add the message into the file before the tag </main_chat>
     * 4. send to a sender the result of the procedure
     * 5. send to other clients the message
     */
    
    // chat message structure
    struct chat_message_s {
        char name[LENGTH_NICKNAME];
        char text[LENGTH_CHAT_MESSAGE];
        char id[6];
    } chat_message;
    
    char username[LENGTH_NICKNAME];
    char message[LENGTH_CHAT_MESSAGE];
    
    memset (username, '\0', LENGTH_NICKNAME);
    memset (message, '\0', LENGTH_CHAT_MESSAGE);
    
    strncpy (username, buffer, LENGTH_NICKNAME);
    strncpy (message, buffer + (LENGTH_NICKNAME + 1), LENGTH_CHAT_MESSAGE);
    
    // parsing XML file
    while (doc != NULL) {}
    if ((doc = xmlParseFile (DATABASE)) == NULL) {
        printf ("%s", MSG_ERROR_WORK_XML);
        xmlSaveFormatFile (DATABASE, doc, 1);
        xmlFreeDoc(doc);
        return NULL;
    }
    xmlNodePtr cur = xmlDocGetRootElement (doc);
    
    printf ("\t\tFile is parsed!\n");
    
    // finding a <main_chat> tag in XML
    cur = cur->children;
    while (xmlStrcmp (cur->name, (xmlChar *) "main_chat") != 0)
        cur = cur->next;
    
    printf ("\t\tTag is found!\n");
    
    char char_id[6];
    memset (char_id, '\0', 6);
    
    // finding the last ID value among all messages
    xmlNodePtr ptr = cur->children;
    while (ptr != NULL) {
        if (xmlStrcmp (ptr->name, (xmlChar *) "text") == 0) {
            ptr = ptr->next;
            continue;
        }
        
        strncpy (char_id, (char *) xmlGetProp (ptr, "id"), strlen ((char *) xmlGetProp (ptr, "id")));
        
        ptr = ptr->next;
    }
    
    printf ("\t\tThe last ID is found\n");
    
    int id = atoi (char_id);
    sprintf (char_id, "%d", ++id);
    
    xmlNodePtr newnode = xmlNewTextChild (cur, NULL, "message", NULL);
    xmlNewProp (newnode, "name", username);
    xmlNewProp (newnode, "text", message);
    xmlNewProp (newnode, "id", char_id);
    
    xmlSaveFormatFile (DATABASE, doc, 1);
    xmlFreeDoc(doc);
    doc = NULL;
    
    printf ("\t\tNew message is added to the database\n");
    
    message_t response;
    memset ((char *) &response, '\0', sizeof (message_t));
    response.type = RESPONSE_CHAT_MESSAGE;
    strncpy (response.buffer, username, LENGTH_NICKNAME);
    strncpy (response.buffer + (LENGTH_NICKNAME + 1), message, LENGTH_CHAT_MESSAGE);
    
    client_list *tmp = first_client;
    while (tmp != NULL) {
        if (np->socket_fd != tmp->socket_fd) { // all clients except itself.
            send(tmp->socket_fd, (void *) &response, sizeof (message_t), 0);
            printf("\t\tMessage (%s) is sent to %s/%s\n", response.buffer + (LENGTH_NICKNAME + 1), tmp->name, tmp->ip);
        }
        tmp = tmp->next;
    }
    
    memset ((char *) &chat_message, '\0', sizeof (chat_message));
    strncpy (chat_message.name, username, strlen (username));
    strncpy (chat_message.text, message, strlen (message));
    strncpy (chat_message.id, char_id, strlen (char_id));
    
    message_t *new_response = (message_t *) malloc (sizeof (message_t));
    memset ((char *) &new_response, '\0', sizeof (message_t));
    new_response->type = 150;
    strncpy (new_response->buffer, (char *) &chat_message, sizeof (chat_message));
    
    return new_response;
}
// processing a request for sending a file
//void send_file_request () {
    
//}

// processing a request for changing a message
message_t* change_message_request (client_list *np, char * buffer) {
    // parsing XML file
    while (doc != NULL) {}
    if ((doc = xmlParseFile (DATABASE)) == NULL) {
        printf ("%s", MSG_ERROR_WORK_XML);
        xmlSaveFormatFile (DATABASE, doc, 1);
        xmlFreeDoc(doc);
        return NULL;
    }
    xmlNodePtr cur = xmlDocGetRootElement (doc);
    
    printf ("\t\tFile is parsed!\n");
    
    // finding a <main_chat> tag in XML
    cur = cur->children;
    while (xmlStrcmp (cur->name, (xmlChar *) "main_chat") != 0)
        cur = cur->next;
    
    printf ("\t\tTag is found!\n");
    
    char char_id[6];
    memset (char_id, '\0', 6);
    strncpy (char_id, buffer, 6);
    
    printf ("\t\tId: %s\n", char_id);
    
    // finding the specific ID value among all messages
    xmlNodePtr ptr = cur->children;
    while (ptr != NULL) {
        if (xmlStrcmp (ptr->name, (xmlChar *) "text") == 0) {
            ptr = ptr->next;
            continue;
        }
        
        if (xmlStrcmp (xmlGetProp (cur, "id"), (xmlChar *) char_id) == 0)
            break;
        
        ptr = ptr->next;
    }
    
    printf ("\t\tMessage is found\n");
    
    char new_message[LENGTH_CHAT_MESSAGE];
    memset (new_message, '\0', LENGTH_CHAT_MESSAGE);
    strncpy (new_message, buffer + 7, LENGTH_CHAT_MESSAGE);
    
    printf ("\t\tNew message is %s\n", new_message);
    
    xmlSetProp (cur, (xmlChar *) "text", (xmlChar *) new_message);
    
    xmlSaveFormatFile (DATABASE, doc, 1);
    xmlFreeDoc(doc);
    doc = NULL;
    
    message_t response;
    response.type = 50;
    
    client_list *tmp = first_client;
    while (tmp != NULL) {
        if (np->socket_fd != tmp->socket_fd) { // all clients except itself.
            send(tmp->socket_fd, (void *) &response, sizeof (message_t), 0);
            printf("\t\tMessage (%s) is sent to %s/%s\n", response.buffer + (LENGTH_NICKNAME + 1), tmp->name, tmp->ip);
        }
        tmp = tmp->next;
    }
    
    
    message_t *new_response = (message_t *) malloc (sizeof (message_t));
    new_response->type = 100;
    
    return new_response;
}
/*
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

/*
void send_to_all_clients(client_list *np, char tmp_buffer[]) {
    client_list *tmp = first_client;
    while (tmp != NULL) {
        if (np->socket_fd != tmp->socket_fd) { // all clients except itself.
            printf("Send to sockfd %d: \"%s\" \n", tmp->socket_fd, tmp_buffer);
            send(tmp->socket_fd, tmp_buffer, LENGTH_SEND, 0);
        }
        tmp = tmp->next;
    }
}
*/




//
client_list *new_node (int sockfd, char *ip) {
    client_list *np = (client_list *) malloc (sizeof (client_list));
    np->socket_fd = sockfd;
    np->prev = NULL;
    np->next = NULL;
    memset (np->name, '\0', sizeof (np->name));
    memset (np->ip, '\0', sizeof (np->ip));
    strncpy (np->ip, ip, strlen (ip));

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