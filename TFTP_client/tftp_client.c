#include "tftp.h"
#include "tftp_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

/* Global variable to store selected transfer mode */
int transfer_mode = MODE_DEFAULT;

int main()
{   
    tftp_client_t client;
    memset(&client, 0, sizeof(client));  // Initialize client structure

    // Main loop for command-line interface
    while (1)
    {
        //print the menu
        printf("......................Menu....................\n");
        printf("1.Connect\n");
        printf("2.Put\n");
        printf("3.Get\n");
        printf("4.Mode\n");
        printf("5.Exit\n");

        //read the choice
        int option;
        int mode;
        printf("Enter your option from menu:");
        scanf("%d",&option);

        //based on the choice perform operation
        switch(option)
        {
            case 1:
                {
                    /* Connect to server */
                    connect_to_server(&client);   
                    break;
                }
            case 2:
                {
                    /* PUT operation */
                    char filename[256];
                    printf("Enter the filename:");
                    getchar();
                    scanf("%[^\n]",filename);
                    getchar();
                    put_file(&client, filename);
                    break;
                }
            case 3:
                {
                    /* GET operation */
                    char filename[256];
                    printf("Enter filename: ");
                    getchar();
                    scanf("%[^\n]", filename);
                    get_file(&client, filename);
                    break;
                }
            case 4:
                {
                    /* Select transfer mode */
                    printf("\nSelect Mode\n");
                    printf("0. Default (512 bytes)\n");
                    printf("1. Octet (1 byte)\n");
                    printf("2. NetASCII\n");

                    printf("Enter mode:\n");
                    scanf("%d", &transfer_mode);

                    if(transfer_mode < 0 || transfer_mode > 2) 
                    {
                        printf("Invalid mode\n");
                        transfer_mode = MODE_DEFAULT;      //set mode as default
                    }
                    break;
                }
            case 5:
                {
                    /* Exit */
                    exit(0);
                }
             default:
             {
                  printf("Invalid main menu option\n");
                  return 1;
             }
        }
    }

    return 0;
}

// This function is to initialize socket with given server IP, no packets sent to server in this function
void connect_to_server(tftp_client_t *client)
{
    // Create UDP socket
    client -> sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (client->sockfd < 0)
    {
        perror("Socket creation failed");
        printf("Please connect to server first\n");
        return;
    }

    char ip[50];

    // read the server address and port no
    printf("Enter Server IP Address: ");
    scanf("%s",ip);

    //validate IP address
    int i = 0;
    int dot_count = 0;
    int digit_count = 0;

    strcpy(client -> server_ip,ip);

    while(client -> server_ip[i])
    {
        if(client -> server_ip[i] >= '0' && client -> server_ip[i] <= '9')
        {
            digit_count++;
        }
        else if(client -> server_ip[i] == '.')
        {
            // dot can't be first numeric character
            if(client -> server_ip[0] == '.')
            {
                printf("Invalid IP address\n");
                return;
            }

            //after dot atleast one numeric char should present
            if(!(client -> server_ip[i+1] >= '0' && client -> server_ip[i+1] <= '9'))
            {
                printf("Invalid IP address\n");
                return;
            }

            dot_count++;
            digit_count = 0;  // reset for next 
        }
        else
        {
            // invalid character
            printf("Invalid IP address\n");
            return;
        }
        i++;
    }

    /* IP must contain exactly 3 dots */
    if(dot_count != 3 || digit_count == 0)
    {
        printf("Invalid IP address\n");
        return;
    }

    int port;

    printf("Enter Server Port Number: ");
    scanf("%d", &port);

    client -> server_port = port;

    //validate port number
    if(!(client -> server_port >= 1024 && client -> server_port <= 65535))
    {
        printf("Invalid port number\n");
        return;
    }

    // store server information
    client -> server_addr.sin_family = AF_INET;
    client -> server_addr.sin_port = htons(client -> server_port);
    client -> server_addr.sin_addr.s_addr = inet_addr(client -> server_ip);

    client->server_len = sizeof(client->server_addr);

    printf("Connected to IP address %s and port %d\n",client->server_ip, client->server_port);
}

void put_file(tftp_client_t *client, char *filename)
{
    //validate the filename is exist or not
    int fd = open(filename, O_RDONLY);

    if(fd == -1)
    {
        perror("File not existed\n");
        return;
    }

    close(fd);

    //if file exists call function and send filename and operation type
    send_request(client -> sockfd, client -> server_addr, filename, WRQ);
    
    // Receive ACK from server
    struct sockaddr_in servinfo;
    socklen_t servlen = sizeof(servinfo);
    
    char ack_status[20];

    int n = recvfrom(client->sockfd, ack_status ,sizeof(ack_status) ,0 ,(struct sockaddr *)&servinfo, &servlen);
    ack_status[n] = '\0';

    if(n < 0)
    {
        perror("Failed to receive ACK from server");
        return;
    }

    // if ack is success call send_file()
    if((strcmp(ack_status,"SUCCESS")) == 0)
    {
        send_file(client->sockfd, servinfo, servlen, filename);
    }
    else
    {
        //if failure print error msg
        printf("data not sent properly!\n");
        return;
    }
}

void get_file(tftp_client_t *client, char *filename)
{
    // Send RRQ 
    send_request(client->sockfd, client->server_addr, filename, RRQ);
    
    /* receive file from server */
    receive_file(client->sockfd, client->server_addr, client->server_len, filename);
}

void disconnect(tftp_client_t *client) {
    // close fd
   
}
void send_request(int sockfd, struct sockaddr_in server_addr, char *filename, int opcode)
{
    tftp_packet packet;

    memset(&packet, 0, sizeof(packet));    // Initialize packet structure

    //convering network byte order
    packet.opcode = htons(opcode);
    
    strncpy(packet.body.request.filename, filename,
            sizeof(packet.body.request.filename) - 1);

    sendto(sockfd, &packet, sizeof(packet), 0,
           (struct sockaddr *)&server_addr, sizeof(server_addr));
}