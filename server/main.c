/* 
 * File:   main.c
 * Author: Alexander Lisitzin
 *
 * Created on July 17, 2019, 10:35 PM
 */

#include "server.h"

int exit_flag = 0;
int server_sock;
struct sockaddr_in client_info;
int requests_number = 0;
xmlDocPtr doc = NULL;
client_list *last_client = NULL;
client_list *first_client = NULL;

int main (void) {
    FILE *db;
    if ((db = fopen (DATABASE, "r")) == NULL) {
            printf ("File database.xml doesn't exists! It will be created right now.\n");
            db = fopen (DATABASE, "w");
            fputs ("<?xml version=\"1.0\"?>\n", db);
            fputs ("<database>\n", db);
            fputs ("\t<users>\n", db);
            fputs ("\t</users>\n", db);
            fputs ("\t<groups>\n", db);
            fputs ("\t</groups>\n", db);
            fputs ("\t<main_chat>\n", db);
            fputs ("\t</main_chat>\n", db);
            fputs ("\t<individual_chat>\n", db);
            fputs ("\t</individual_chat>\n", db);
            fputs ("</database>\n", db);
    } else
            printf ("File database.xml already exists!\n");
    fclose (db);
    
    printf ("To close the server, enter /exit\n");
    
    // create a server socket
    if ((server_sock = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
        return (EXIT_FAILURE);
    }
    
    // fill information about the server address
    memset ((char *) &client_info, '\0', sizeof (client_info));
    client_info.sin_family = PF_INET;
    client_info.sin_addr.s_addr = inet_addr ("127.0.0.1");
    client_info.sin_port = htons (8888);
    
    // bind address to a socket
    int server_addr_len = sizeof (client_info);
    bind (server_sock, (struct sockaddr*) &client_info, server_addr_len);
    // listen by a socket
    listen (server_sock, 5);
    
    // create a thread for entering commands
    pthread_t command_thread_id;
    if (pthread_create (&command_thread_id, NULL, (void *) command_handler, NULL) != 0) {
        printf ("\r%s", MSG_ERROR_THREADS);
        exit_flag = 1;
    } else {
        // create a thread for connecting of clients
        pthread_t client_connecting_thread_id;
        if (pthread_create (&client_connecting_thread_id, NULL, (void *) client_connecting_handler, NULL) != 0) {
            printf ("\r%s", MSG_ERROR_THREADS);
            exit_flag = 1;
        }
    }
    
    while (exit_flag != 1) {}
    
    while (requests_number > 0) {}
    
    return (EXIT_SUCCESS);
}

