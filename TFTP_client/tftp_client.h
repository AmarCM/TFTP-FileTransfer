#ifndef TFTP_CLIENT_H
#define TFTP_CLIENT_H

#include <fcntl.h>
#include <string.h>
#include <stdio.h>

//to collect information about client
typedef struct
{
    int sockfd;
    struct sockaddr_in server_addr;
    socklen_t server_len;               // length 
    char server_ip[INET_ADDRSTRLEN];    //to store IP
    int server_port;                    //to store port num 
} tftp_client_t;      

// Function prototypes
//void connect_to_server(tftp_client_t *client, char *ip, int port);
void connect_to_server(tftp_client_t *client);
void put_file(tftp_client_t *client, char *filename);
void get_file(tftp_client_t *client, char *filename);
void disconnect(tftp_client_t *client);                     
//void process_command(tftp_client_t *client, char *command);

//use while doing put operation
void send_request(int sockfd, struct sockaddr_in server_addr, char *filename, int opcode);

//use while doing get operation
void receive_request(int sockfd, struct sockaddr_in server_addr, char *filename, int opcode);

#endif