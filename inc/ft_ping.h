#ifndef FT_PING_H
# define FT_PING_H

# define EXIT_FAILURE 1
# define INET "nordlynx"

# include <stdint.h>

typedef struct s_data
{   
    struct ip_header
    {
        unsigned char   version_ihl;    // Version (4 bits) + IHL (4 bits)
        unsigned char   tos;            // Type of Service (8 bits)
        unsigned short  total_length;   // Total length of the packet (16 bits)
        unsigned short  id;             // Identification (16 bits)
        unsigned short  flags_frag;       // Fragment offset (16 bits)
        unsigned char   ttl;            // Time To Live (8 bits)
        unsigned char   protocol;       // Protocol (8 bits)
        unsigned short  checksum;       // Header checksum (16 bits)
        unsigned int    src_ip;         // Source IP address (32 bits)
        unsigned int    dest_ip;        // Destination IP address (32 bits)
    } ip_header;

    struct icmp_header
    {
        unsigned char   type;        // Type (1 byte)
        unsigned char   code;        // Code (1 byte)
        unsigned short  checksum;   // Checksum (2 bytes)
        unsigned short  id;         // Identifier (2 bytes)
        unsigned short  seq_num;    // Sequence Number (2 bytes)
        unsigned char   *data;       // Optional Data (variable length)

    } icmp_header;

    unsigned char       *packet;
    int                 socket;
    struct sockaddr_in     *dest_addr;

} t_data;

void    init(t_data *data, char *dst);
void    generate_packet(t_data *data);
void    allocate_and_send(t_data *data);

void    error(char *str);
void    print_addr(uint32_t ip);
void    print_headers(t_data *data);
void    print_binary(unsigned char *addr, size_t size);

#endif