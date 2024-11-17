#ifndef FT_PING_H
# define FT_PING_H

# define EXIT_FAILURE 1
# define INET "nordlynx"

typedef struct s_data
{   
    struct ip_header
    {
        unsigned char  version_ihl;    // Version (4 bits) + IHL (4 bits)
        unsigned char  tos;            // Type of Service (8 bits)
        unsigned short total_length;   // Total length of the packet (16 bits)
        unsigned short id;             // Identification (16 bits)
        unsigned short frag_off;       // Fragment offset (16 bits)
        unsigned char  ttl;            // Time To Live (8 bits)
        unsigned char  protocol;       // Protocol (8 bits)
        unsigned short checksum;       // Header checksum (16 bits)
        unsigned int   src_ip;         // Source IP address (32 bits)
        unsigned int   dest_ip;        // Destination IP address (32 bits)
    } ip_header;

} t_data;

void    init(t_data *data, char *dst);

void    error(char *str);
void    print_addr(uint32_t ip);

#endif