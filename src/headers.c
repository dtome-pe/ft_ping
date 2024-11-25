#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "../inc/ft_ping.h"

void    generate_headers(t_data *data)
{   
    /*--------------------------------------*/
    /*IP HEADER*/
    /*--------------------------------------*/

    /*SET VERSION AND INTERNET HEADER LENGTH*/

    data->ip_hdr.version = 4; //ipv4
    data->ip_hdr.ihl = 5;  // default no-options length

    /*-------------------------------------*/

    /*SET TYPE OF SERVICE*/

    data->ip_hdr.tos = 0x00;

    /*-------------------------------------*/

    /*SET TOTAL LENGTH*/

    data->ip_hdr.tot_len = htons(84);

    /*-------------------------------------*/

    /*SET IDENTIFICATION */

    unsigned short id = rand() % 65536; // Random value between 0 and 65535
    data->ip_hdr.id = htons(id);

    /*-------------------------------------*/

    /*SET FLAGS AND FRAGMENTATION OFFSET*/

    data->ip_hdr.frag_off = htons(0x4000);  // flags-> don't fragment   fragmentation offset->0

    /*-------------------------------------*/

    /*SET TIME TO LIVE*/

    data->ip_hdr.ttl = data->opts.ttl; //64 - unix standard

	//data->ip_hdr.ttl = 0x00; // force error

    /*-------------------------------------*/

    /*SET PROTOCOL*/

    data->ip_hdr.protocol = 0x01; //icmp

    /*-------------------------------------*/

    /*INITIALIZE AND CALCULATE CHECK_SUM*/

    data->ip_hdr.check = 0x0000;  // should be zero before checksum

    data->ip_hdr.check = calculate_checksum(&data->ip_hdr, sizeof(data->ip_hdr));

    /*-------------------------------------*/
    /*ICMP HEADER*/
    /*-------------------------------------*/

    /*SET TYPE OF MESSAGE*/

    data->icmp_hdr.type = 0x08;

    /*------------------------------------*/

    /*SET CODE*/

    data->icmp_hdr.code = 0x00;

    /*------------------------------------*/

    /*SET CHECKSUM*/

    data->icmp_hdr.checksum = 0x0000;

    /*------------------------------------*/

    /*SET IDENTIFIER*/

    data->icmp_hdr.un.echo.id = htons(getpid() & 0xFFFF);

    /*-----------------------------------*/

    /*SET SEQUENCE NUMBER*/

    data->icmp_hdr.un.echo.sequence = htons(data->stats.seq);

    /*-----------------------------------*/

}
