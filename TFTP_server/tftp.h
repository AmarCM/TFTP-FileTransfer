/* Common file for server & client*/

#ifndef TFTP_H
#define TFTP_H

#include <stdint.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#define MODE_DEFAULT   0
#define MODE_OCTET     1
#define MODE_NETASCII  2

extern int transfer_mode;

#define PORT 6969
#define BUFFER_SIZE 516  // TFTP data packet size (512 bytes data + 4 bytes header)
#define TIMEOUT_SEC 5    // Timeout in seconds

// TFTP OpCodes
typedef enum {
    RRQ = 1,  // Read Request
    WRQ = 2,  // Write Request
    DATA = 3, // Data Packet
    ACK = 4,  // Acknowledgment
    ERROR = 5 // Error Packet
} tftp_opcode;

// TFTP Packet Structure
typedef struct
{
    uint16_t opcode; // Operation code (RRQ/WRQ/DATA/ACK/ERROR)
    union
    {
        struct 
        {
            char filename[256];
            char mode[8];  // Typically "octet"  (0:normal mode 1: octet mode and 2:netascii mode)
        } request;  // RRQ and WRQ
        struct
        {
            uint16_t block_number;
            char data[512];
            int data_size;
        } data_packet; // DATA
        struct
        {
            uint16_t block_number;
            int data_size;
        } ack_packet; // ACK
        struct
        {
            uint16_t error_code;
            char error_msg[512];
        } error_packet; // ERROR
    } body;
} tftp_packet;

//common for both client and server
void send_file(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, char *filename);
void receive_file(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, char *filename);


#endif // TFTP_H
