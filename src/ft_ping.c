/*
 * C Program to Get IP Address
 */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../inc/ft_ping.h"

int main(int argc, char **argv)
{
    if (argc != 2)
        error("Wrong number of arguments\n");

    char *ping_datagram;

    ping_datagram = generate_packet(argv[1]);
    
    return 0;
}