#include "../inc/ft_ping.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double calculate_stddev(struct rtt_node *rtt_list_head, int n, double avg_rtt) {
    if (n <= 1) {
        return 0.0;  // Standard deviation is 0 if there's only one value
    }

    double sum_sq_diff = 0.0;

    struct rtt_node *current = rtt_list_head;

    while (current) {
        double diff = current->rtt_value - avg_rtt;
        sum_sq_diff += diff * diff;
        current = current->next;  // Move to the next node
    }

    return sqrt(sum_sq_diff / n);
}

void end(t_data *data)
{

        free(data->dest_addr);
        double  rtt_avg = data->stats.total_rtt / data->stats.packets_received;
        double  stdev = calculate_stddev(data->stats.rtt_list_head, data->stats.rtt_count, rtt_avg);

        fflush(stdout);
        printf("\n--- %s ping statistics ---\n", data->hostname);
        printf("%d packets transmitted, %d received, %.2f%% packet loss\n",
                data->stats.packets_sent, data->stats.packets_received,
               100.0 * (1 - ((double)data->stats.packets_received / data->stats.packets_sent)));
		if (data->stats.packets_received)
			printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
					data->stats.min_rtt, rtt_avg, data->stats.max_rtt, stdev);
}