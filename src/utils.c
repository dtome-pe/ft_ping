#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>

void    error(char *str)
{
    write(2, str, strlen(str));
    exit(1);
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