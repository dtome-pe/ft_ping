#ifndef FT_PING_H
# define FT_PING_H

# define EXIT_FAILURE 1
# define INET "nordlynx"

# include <sys/time.h>

# include <stdint.h>


typedef struct s_data
{   
    struct timeval timeval;

    struct ip_header
    {
        uint8_t         version_ihl;    // Version (4 bits) + IHL (4 bits)
        uint8_t         tos;            // Type of Service (8 bits)
        uint16_t        total_length;   // Total length of the packet (16 bits)
        uint16_t        id;             // Identification (16 bits)
        uint16_t        flags_frag;       // Fragment offset (16 bits)
        uint8_t         ttl;            // Time To Live (8 bits)
        uint8_t         protocol;       // Protocol (8 bits)
        uint16_t        checksum;       // Header checksum (16 bits)
        uint32_t        src_ip;         // Source IP address (32 bits)
        uint32_t        dest_ip;        // Destination IP address (32 bits)
    } ip_header;

    struct icmp_header
    {
        uint8_t         type;        // Type (1 byte)
        uint8_t         code;        // Code (1 byte)
        uint16_t        checksum;   // Checksum (2 bytes)
        uint16_t        id;         // Identifier (2 bytes)
        uint16_t        seq_num;    // Sequence Number (2 bytes)
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