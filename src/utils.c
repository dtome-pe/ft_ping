#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "../inc/ft_ping.h"

void    error(char *str)
{
    write(2, str, strlen(str));
    exit(1);
}

void print_binary(unsigned char *addr, size_t size) {
    for (size_t i = 0; i < size; i++) {
        for (int j = 7; j >= 0; j--) {
            printf("%d", (addr[i] >> j) & 1);
        }
        if (i < size - 1) printf(" ");
    }
    printf("\n");
}

void print_addr(uint32_t ip) {
    // Print the binary representation of the IP address
    unsigned char *addr = (unsigned char *)&ip;
    for (size_t i = 0; i < sizeof(ip); i++) {
        for (int j = 7; j >= 0; j--) {
            printf("%d", (addr[i] >> j) & 1);
        }
        if (i < sizeof(ip) - 1) printf(" ");
    }
    
    printf("\n");

    // Convert uint32_t to struct in_addr for human-readable format
    struct in_addr ip_addr;
    ip_addr.s_addr = htonl(ip); // Ensure the IP is in network byte order

    // Print the IP address in string format
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ip_addr, ip_str, sizeof(ip_str));

    printf("Human-readable IP: %s\n", ip_str);
}

// Function to print a 16-bit word in binary, hexadecimal, and decimal
void print_16bit_word(uint16_t word, char *name) {
    // Print the word in binary
    printf("%s\n", name);

    printf("Binary: ");
    for (int i = 15; i >= 0; i--) {
        printf("%d", (word >> i) & 1);
    }

    // Print the word in hexadecimal
    printf(" | Hex: 0x%04X", word);

    // Print the word in decimal
    printf(" | Decimal: %d\n", word);
}

// Function to print each 16-bit word of the IP header from t_data structure
void print_headers(t_data *data) 
{
    printf("Printing IP header 16-bit words:\n");

    // Print each 16-bit word of the ip_header
    print_16bit_word(data->ip_header.version_ihl << 8 | data->ip_header.tos, "version, ihl and tos");  // First word (version_ihl + tos)
    print_16bit_word(data->ip_header.total_length, "total length");
    print_16bit_word(data->ip_header.id, "identification");
    print_16bit_word(data->ip_header.flags_frag, "flags and fragmentation offset");
    print_16bit_word(data->ip_header.ttl << 8 | data->ip_header.protocol, "time to live and protocol");  // ttl + protocol
    print_16bit_word(data->ip_header.checksum, "header check_sum");
    print_16bit_word(data->ip_header.src_ip >> 16, "source ip first 16"); // Source IP (high 16 bits)
    print_16bit_word(data->ip_header.src_ip & 0xFFFF, "source ip second 16"); // Source IP (low 16 bits)
    print_16bit_word(data->ip_header.dest_ip >> 16, "dest ip first 16"); // Destination IP (high 16 bits)
    print_16bit_word(data->ip_header.dest_ip & 0xFFFF, "dest ip second 16"); // Destination IP (low 16 bits)

    print_16bit_word((data->icmp_header.type << 8) | data->icmp_header.code, "icmp type and code");
    print_16bit_word(data->icmp_header.checksum, "icmp checksum");
    print_16bit_word(data->icmp_header.id, "icmp identifier");
    print_16bit_word(data->icmp_header.seq_num, "icmp seq numb");
}