#include "../inc/ft_ping.h"
#include <stdio.h>
#include <stdlib.h>


void    end(t_data *data)
{
    
    free(data->dest_addr);

    fflush (stdout);
    printf("\n--- %s ping statistics ---\n", data->hostname);
    printf("%d packets transmitted, %d received, %.2f%% packet loss\n",
            data->stats.packets_sent, data->stats.packets_received,
           100.0 * (1 - ((double)data->stats.packets_received / data->stats.packets_sent)));
}