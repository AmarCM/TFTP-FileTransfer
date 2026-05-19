#include "tftp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

/* Buffer to store requested filename */
char filename[256];

/* Global variable to store selected transfer mode */
int transfer_mode = MODE_DEFAULT;

void handle_client(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, tftp_packet *packet);

int main()
{
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    tftp_packet packet;

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if(sockfd == -1)
    {
        perror("Socket creation failed\n");
        printf("Please connect to server first\n");
        return 1;
    }
    
    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if(bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Binding failed\n");
        return 1;
    }

    printf("TFTP server is waiting for data\n");

    // Main loop to handle incoming requests
    while (1)
    {
        int n = recvfrom(sockfd, &packet, sizeof(packet), 0,
                         (struct sockaddr *)&client_addr, &client_len);

        packet.body.request.filename[sizeof(packet.body.request.filename) - 1] = '\0';

        if(n > 0)
        {
            handle_client(sockfd, client_addr, client_len, &packet);
        }
    }

    close(sockfd);
    return 0;
}

void handle_client(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, tftp_packet *packet) 
{
    // Extract the TFTP operation (read or write) from the received packet
    int op = ntohs(packet -> opcode);

    strncpy(filename, packet->body.request.filename, sizeof(filename) - 1);
    filename[sizeof(filename) - 1] = '\0';

    if(op == WRQ)
    {
        printf("Request received: operation = %d\n", op);
        printf("Request received: filename = %s\n", filename);

        //check file exist or not
        int fd = open(filename, O_CREAT | O_TRUNC | O_WRONLY, 0666);

        if(fd == -1)
        {
            perror("File opened failed\n");
            sendto(sockfd,"FAILURE", 7, 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
            return;
        }

        close(fd);
        
        //send ack back to client about file exist
        sendto(sockfd,"SUCCESS", 7, 0, (struct sockaddr *)&client_addr, sizeof(client_addr));

        printf("Started receiving file: %s\n", filename);

        // and call send_file or receive_file accordingly
        receive_file(sockfd, client_addr, client_len, filename);
    }
    else if(op == RRQ)
    {
        printf("RRQ received for file: %s\n", filename);

        int fd = open(filename, O_RDONLY);
        if (fd == -1)
        {
            perror("File not found");
            return;
        }

        close(fd);
        /* Send file data to client */
        send_file(sockfd, client_addr, client_len, filename);
    }
}




