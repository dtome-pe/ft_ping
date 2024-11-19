#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>

#include "../inc/ft_ping.h"

void    generate_packet(t_data *data)
{   
    /*--------------------------------------*/
    /*IP HEADER*/
    /*--------------------------------------*/

    /*SET VERSION AND INTERNET HEADER LENGTH*/

    unsigned char version = 4 << 4; //ipv4
    unsigned char ihl = 5;  // default no-options length

    data->ip_header.version_ihl = version | ihl;

    /*-------------------------------------*/

    /*SET TYPE OF SERVICE*/

    data->ip_header.tos = 0x00;

    /*-------------------------------------*/

    /*SET TOTAL LENGTH*/

    data->ip_header.total_length = htons(84);

    /*-------------------------------------*/

    /*SET IDENTIFICATION */

    unsigned short id = rand() % 65536; // Random value between 0 and 65535
    data->ip_header.id = htons(id);

    /*-------------------------------------*/

    /*SET FLAGS AND FRAGMENTATION OFFSET*/

    data->ip_header.flags_frag = htons(0x4000);  // flags-> don't fragment   fragmentation offset->0

    /*-------------------------------------*/

    /*SET TIME TO LIVE*/

    data->ip_header.ttl = 0x40; //64 - unix standard

    /*-------------------------------------*/

    /*SET PROTOCOL*/

    data->ip_header.protocol = 0x01; //icmp

    /*-------------------------------------*/

    /*INITIALIZE CHECK_SUM*/

    data->ip_header.checksum = 0x0000;  // should be zero before checksum

    /*-------------------------------------*/

    /*IP HEADER CHECKSUM*/

    uint32_t    sum = 0;

    // Sum all 16-bit words of the IP header (we break it into 16-bit words)
    sum += (data->ip_header.version_ihl << 8) | data->ip_header.tos;   // First 16-bit word
    sum += data->ip_header.total_length;                                // Second 16-bit word
    sum += data->ip_header.id;                                          // Third 16-bit word
    sum += data->ip_header.flags_frag;                                  // Fourth 16-bit word
    sum += (data->ip_header.ttl << 8) | data->ip_header.protocol;       // Fifth 16-bit word
    sum += data->ip_header.checksum;                                    // Sixth 16-bit word
    sum += (data->ip_header.src_ip >> 16);                               // Seventh 16-bit word (high part of src_ip)
    sum += (data->ip_header.src_ip & 0xFFFF);                            // Eighth 16-bit word (low part of src_ip)
    sum += (data->ip_header.dest_ip >> 16);                              // Ninth 16-bit word (high part of dest_ip)
    sum += (data->ip_header.dest_ip & 0xFFFF);                           // Tenth 16-bit word (low part of dest_ip)

    // Handle the carry (if any), wrap around any 16-bit overflow
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);  // Add the carry back to the sum
    }

    // One's complement the sum (invert the bits)
    data->ip_header.checksum = ~sum;

    /*-------------------------------------*/
    /*ICMP HEADER*/
    /*-------------------------------------*/

    /*SET TYPE OF MESSAGE*/

    data->icmp_header.type = 0x08;

    /*------------------------------------*/

    /*SET CODE*/

    data->icmp_header.code = 0x00;

    /*------------------------------------*/

    /*SET CHECKSUM*/

    data->icmp_header.checksum = 0x0000;

    /*------------------------------------*/

    /*SET IDENTIFIER*/

    data->icmp_header.id = htons(0x1234);

    /*-----------------------------------*/

    /*SET SEQUENCE NUMBER*/

    data->icmp_header.seq_num = htons(0x0001);

    /*-----------------------------------*/

    /*ICMP HEADER CHECKSUM*/

    sum = 0;

    sum += (data->icmp_header.type << 8) | data->icmp_header.code; // Type and Code (combined as 16 bits)
    sum += data->icmp_header.checksum;                            //checksum (16bits)    
    sum += ntohs(data->icmp_header.id);                                  // Identifier (16 bits)    
    sum += ntohs(data->icmp_header.seq_num);                        // seq number (16 bits)

    data->icmp_header.checksum = htons(~sum);  // Mask to 16 bits

    //printf("Calculated checksum: 0x%04x\n", data->icmp_header.checksum);

    /*---------------------*/

    /*ALLOCATE MEMORY FOR PACKET*/
    data->packet_size = sizeof(data->ip_header) + sizeof(data->icmp_header);

    printf("packet size: %ld\n", data->packet_size);
    data->packet = malloc(data->packet_size);
    if (!data->packet) {
        perror("Packet allocation failed");
        exit(EXIT_FAILURE);
    }
    memset(data->packet, 0, data->packet_size); // Initialize all bytes to zero

    /*--------------------------*/

    /*COPY BOTH HEADERS INTO PACKET*/
    memcpy(data->packet, &data->ip_header, sizeof(data->ip_header));
    memcpy(data->packet + sizeof(data->ip_header), &data->icmp_header, sizeof(data->icmp_header));

    /*---------------------------*/
}

