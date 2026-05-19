/* Common file for server & client */

#include "tftp.h"
#include <fcntl.h>
#include <unistd.h>

void send_file(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("File open failed\n");
        return;
    }

    /* Packet structures for DATA and ACK */
    tftp_packet pkt, ack;
    uint16_t block = 1;

    while (1)
    {
        /* Clear packet before filling */
        memset(&pkt, 0, sizeof(pkt));

        pkt.opcode = htons(DATA);
        pkt.body.data_packet.block_number = htons(block);
        pkt.body.data_packet.data_size = read(fd, pkt.body.data_packet.data, 512);

        int bytes = pkt.body.data_packet.data_size;

    resend:
        /* Send DATA packet */
        sendto(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr *)&client_addr, client_len);

        /* wait for ACK */
        recvfrom(sockfd, &ack, sizeof(ack), 0, (struct sockaddr *)&client_addr, &client_len);

        /* Validate ACK block number and data size */
        if(ntohs(ack.opcode) == ACK && ntohs(ack.body.ack_packet.block_number) == block && ack.body.ack_packet.data_size == bytes)
        {
            block++; //move for next block / bytes of data 
        }
        else
        {
            goto resend;    //resend the same data
        }

        /* EOF */
        if(bytes == 0)
        {
            break;
        }
    }

    close(fd);
}

void receive_file(int sockfd, struct sockaddr_in client_addr,socklen_t client_len, char *filename)
{
    /* Create or overwrite file for writing */
    int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0666);

    if(fd == -1)
    {
        perror("File create failed\n");
        return;
    }

    tftp_packet pkt, ack;
    uint16_t expected_block = 1;

    while (1)
    {
        /* Receive DATA packet */
        recvfrom(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr *)&client_addr, &client_len);

        /* Ignore non-DATA packets */
        if(ntohs(pkt.opcode) != DATA)
            continue;

        /* Extract block number */
        uint16_t block = ntohs(pkt.body.data_packet.block_number);

        /* Extract received data size */
        int bytes = pkt.body.data_packet.data_size;

        if(block == expected_block)
        {
            write(fd, pkt.body.data_packet.data, bytes);
            expected_block++;
        }

        /* send ACK */
        memset(&ack, 0, sizeof(ack));
        ack.opcode = htons(ACK);
        ack.body.ack_packet.block_number = htons(block);
        ack.body.ack_packet.data_size = bytes;

        sendto(sockfd, &ack, sizeof(ack), 0, (struct sockaddr *)&client_addr, client_len);

        if(bytes == 0)
        {
            printf("File transfer completed...\n");
            break;
        }
    }

    close(fd);
}
