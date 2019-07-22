/* 
 * File:   main.c
 * Author: Alexander Lisitzin
 *
 * Created on July 17, 2019, 10:35 PM
 */

/*
 #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "proto.h"
#include "server.h"

// Global variables
int server_sockfd = 0, client_sockfd = 0;
ClientList *root, *now;

void catch_ctrl_c_and_exit(int sig) {
    ClientList *tmp;
    while (root != NULL) {
        printf("\nClose socketfd: %d\n", root->data);
        close(root->data); // close all socket include server_sockfd
        tmp = root;
        root = root->link;
        free(tmp);
    }
    printf("Bye\n");
    exit(EXIT_SUCCESS);
}

void send_to_all_clients(ClientList *np, char tmp_buffer[]) {
    ClientList *tmp = root->link;
    while (tmp != NULL) {
        if (np->data != tmp->data) { // all clients except itself.
            printf("Send to sockfd %d: \"%s\" \n", tmp->data, tmp_buffer);
            send(tmp->data, tmp_buffer, LENGTH_SEND, 0);
        }
        tmp = tmp->link;
    }
}

void client_handler(void *p_client) {
    int leave_flag = 0;
    char nickname[LENGTH_NAME] = {};
    char recv_buffer[LENGTH_MSG] = {};
    char send_buffer[LENGTH_SEND] = {};
    ClientList *np = (ClientList *)p_client;

    // Naming
    if (recv(np->data, nickname, LENGTH_NAME, 0) <= 0 || strlen(nickname) < 2 || strlen(nickname) >= LENGTH_NAME-1) {
        printf("%s didn't input name.\n", np->ip);
        leave_flag = 1;
    } else {
        strncpy(np->name, nickname, LENGTH_NAME);
        printf("%s(%s)(%d) join the chatroom.\n", np->name, np->ip, np->data);
        sprintf(send_buffer, "%s(%s) join the chatroom.", np->name, np->ip);
        send_to_all_clients(np, send_buffer);
    }

    // Conversation
    while (1) {
        if (leave_flag) {
            break;
        }
        int receive = recv(np->data, recv_buffer, LENGTH_MSG, 0);
        if (receive > 0) {
            if (strlen(recv_buffer) == 0) {
                continue;
            }
            sprintf(send_buffer, "%s：%s from %s", np->name, recv_buffer, np->ip);
        } else if (receive == 0 || strcmp(recv_buffer, "exit") == 0) {
            printf("%s(%s)(%d) leave the chatroom.\n", np->name, np->ip, np->data);
            sprintf(send_buffer, "%s(%s) leave the chatroom.", np->name, np->ip);
            leave_flag = 1;
        } else {
            printf("Fatal Error: -1\n");
            leave_flag = 1;
        }
        send_to_all_clients(np, send_buffer);
    }

    // Remove Node
    close(np->data);
    if (np == now) { // remove an edge node
        now = np->prev;
        now->link = NULL;
    } else { // remove a middle node
        np->prev->link = np->link;
        np->link->prev = np->prev;
    }
    free(np);
}

int main()
{
    signal(SIGINT, catch_ctrl_c_and_exit);

    // Create socket
    server_sockfd = socket(AF_INET , SOCK_STREAM , 0);
    if (server_sockfd == -1) {
        printf("Fail to create a socket.");
        exit(EXIT_FAILURE);
    }

    // Socket information
    struct sockaddr_in server_info, client_info;
    int s_addrlen = sizeof(server_info);
    int c_addrlen = sizeof(client_info);
    memset(&server_info, 0, s_addrlen);
    memset(&client_info, 0, c_addrlen);
    server_info.sin_family = PF_INET;
    server_info.sin_addr.s_addr = INADDR_ANY;
    server_info.sin_port = htons(8888);

    // Bind and Listen
    bind(server_sockfd, (struct sockaddr *)&server_info, s_addrlen);
    listen(server_sockfd, 5);

    // Print Server IP
    getsockname(server_sockfd, (struct sockaddr*) &server_info, (socklen_t*) &s_addrlen);
    printf("Start Server on: %s:%d\n", inet_ntoa(server_info.sin_addr), ntohs(server_info.sin_port));

    // Initial linked list for clients
    root = newNode(server_sockfd, inet_ntoa(server_info.sin_addr));
    now = root;

    while (1) {
        client_sockfd = accept(server_sockfd, (struct sockaddr*) &client_info, (socklen_t*) &c_addrlen);

        // Print Client IP
        getpeername(client_sockfd, (struct sockaddr*) &client_info, (socklen_t*) &c_addrlen);
        printf("Client %s:%d come in.\n", inet_ntoa(client_info.sin_addr), ntohs(client_info.sin_port));

        // Append linked list for clients
        ClientList *c = newNode(client_sockfd, inet_ntoa(client_info.sin_addr));
        c->prev = now;
        now->link = c;
        now = c;

        pthread_t id;
        if (pthread_create(&id, NULL, (void *)client_handler, (void *)c) != 0) {
            perror("Create pthread error!\n");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
 * 

 */

#include "server.h"

int exit_flag = 0;
int server_sock;
struct sockaddr_in server_addr;
int threads_number = 0;
xmlDocPtr doc = NULL;
client_list *now = NULL;

int main (void) {
/*
    Dont forget to refactor!!!
*/
    FILE *db;
    if ((db = fopen (DATABASE, "r")) == NULL) {
            printf ("database.xml doesn't exists! It will be created right now.\n");
            db = fopen (DATABASE, "w");
            fputs ("<?xml version=\"1.0\"?>\n", db);
            fputs ("<database>\n", db);
            fputs ("\t<users>\n", db);
            fputs ("\t</users>\n", db);
            fputs ("\t<groups>\n", db);
            fputs ("\t</groups>\n", db);
            fputs ("\t<main_chat>\n", db);
            fputs ("\t</main_chat>\n", db);
            fputs ("</database>\n", db);
    } else
            printf ("database.xml exists!\n");
    fclose (db);
    
    // create a server socket
    if ((server_sock = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
        return (EXIT_FAILURE);
    }
    
    // fill information about the server address
    memset ((char *) &server_addr, '\0', sizeof (server_addr));
    server_addr.sin_family = PF_INET;
    server_addr.sin_addr.s_addr = inet_addr ("127.0.0.1");
    server_addr.sin_port = htons (8888);
    
    // bind address to a socket
    int server_addr_len = sizeof (server_addr);
    bind (server_sock, (struct sockaddr*) &server_addr, server_addr_len);
    // listen by a socket
    listen (server_sock, 5);
    
    // create a thread for entering commands
    pthread_t command_thread_id;
    if (pthread_create (&command_thread_id, NULL, (void *) command_handler, NULL) != 0) {
        printf ("\r%s", MSG_ERROR_THREADS);
        exit_flag = 1;
    } else {
        // create a thread for connecting of clients
        printf ("Connecting to a user...\n");
        pthread_t client_connecting_thread_id;
        if (pthread_create (&client_connecting_thread_id, NULL, (void *) client_connecting_handler, NULL) != 0) {
            printf ("\r%s", MSG_ERROR_THREADS);
            exit_flag = 1;
        }
    }
    
    while (exit_flag != 1) {}
    
    while (threads_number > 1) {}
    
    return (EXIT_SUCCESS);
}
