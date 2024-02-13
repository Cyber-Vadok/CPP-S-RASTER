// Created by Roberto Vadacca.

#define DEBUG

// c include
#include <stdio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// c++ include
#include <cstdint>
#include <string>
#include <iostream>
#include <chrono>

// my include
#include "tile.h"
#include "cluster.h"
#include "generator.h"

#define MAX_PRECISION 14.0 // massima precisione per i valori generati
#define MAX_MU 8           // massimo numero di vicini per una tile

// global variables

std::vector<file_entry> file_entries;

// parametri algoritmo
uint8_t mu = 4;        // Minimum cluster size in terms of the number of signifcant tiles
float precision = 3.5; // Precision for projection operation anche float
uint8_t tau = 5;      // Threshold number of points to determine if a tile is signifcant
uint8_t delta = 1;     // Distance metric for cluster defnition
uint8_t window_size = 10;

// parametri generazione dataset
uint32_t points_per_cluster = 500;
uint32_t number_of_clusters = 1000; // per period
uint32_t periods = 10;
uint64_t seed = 0;
double max_lng = 180.0;
double min_lng = -180.0;
double max_lat = 90.0;
double min_lat = -90.0;
double min_dist = 0.0500; // 50 * 1.1m
// A study based on London, for instance, found
// a mean error of raw GPS measurements of vehicular movements of 53.7 m

// parametri implementazione
bool timer_flag = false; // stampa tempo di esecuzione

std::chrono::high_resolution_clock::time_point start_time;

void usage()
{
    fprintf(stderr, "Usage: ./sraster [-m <mu>] [-p <xi>] [-t <tau>] [-d <delta>] [-w <window size>]\n");
    fprintf(stderr, "  -m <mu>  Set minimum cluster size in terms of the number of signifcant tiles (default: 5, max = %u)\n", UINT8_MAX);
    fprintf(stderr, "  -p <xi>  Set precision for projection operation (default: 5, max = %f)\n", MAX_PRECISION);
    fprintf(stderr, "  -t <tau> Set threshold number of points to determine if a tile is significant (default: 8, max = %u)\n", UINT8_MAX);
    fprintf(stderr, "  -d <delta> Set distance metric for cluster definition (default: 1, max = %u)\n", UINT8_MAX);
    fprintf(stderr, "  -w <window size> Set window size (default: 5)\n");
    fprintf(stderr, "  -s <seed> Set seed for random generator (default: 0)\n");
}

int main(int argc, char **argv)
{

    // argument parser
    // https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html
    int c;
    while ((c = getopt(argc, argv, "m:p:t:d:w:s:hc")) != -1)
    {
        switch (c)
        {
        case 'h':
        {
            usage();
            exit(1);
            break;
        }
        case 'm': // minimum cluster size
        {
            if (std::stoul(optarg) > MAX_MU)
            {
                fprintf(stderr, "Invalid mu value, max = %u\n", MAX_MU);
                usage();
                exit(EXIT_FAILURE);
            }
            else
            {
                mu = std::stoul(optarg);
            }
            break;
        }
        case 'p': // precision
        {
            char *endptr;
            float parsedValue = std::strtof(optarg, &endptr);
            if (endptr == optarg || *endptr != '\0' || parsedValue > MAX_PRECISION)
            {
                fprintf(stderr, "Invalid precision value\n");
                usage();
                exit(EXIT_FAILURE);
            }
            else
            {
                precision = parsedValue;
            }
            break;
        }
        case 't': // threshold
        {
            if (std::stoul(optarg) > UINT8_MAX)
            {
                fprintf(stderr, "Invalid tau value, max = %u\n", UINT8_MAX);
                usage();
                exit(EXIT_FAILURE);
            }
            else
            {
                tau = std::stoul(optarg);
            }
            break;
        }
        case 'd': // distance metric
        {
            if (std::stoul(optarg) > UINT8_MAX)
            {
                fprintf(stderr, "Invalid delta value, max = %u\n", UINT8_MAX);
                usage();
                exit(EXIT_FAILURE);
            }
            else
            {
                delta = std::stoul(optarg);
            }
            break;
        }
        case 'w': // window size
        {
            if (std::stoul(optarg) > UINT8_MAX)
            {
                fprintf(stderr, "Invalid window size value, max = %u\n", UINT8_MAX);
                usage();
                exit(EXIT_FAILURE);
            }
            else
            {
                window_size = std::stoul(optarg);
            }
            break;
        }
        case 'c':
        {
            timer_flag = true;
            break;
        }
        case 's':
        {
            if (std::stoul(optarg) > UINT64_MAX)
            {
                fprintf(stderr, "Invalid seed value, max = %lu\n", UINT64_MAX);
                usage();
                exit(EXIT_FAILURE);
            }
            else
            {
                seed = std::stoul(optarg);
            }
            break;
        }

        case '?':
        {
            if (optopt == 'm' || optopt == 'p' || optopt == 't' || optopt == 'd' || optopt == 'w' || optopt == 's')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint(optopt))
                fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf(stderr,
                        "Unknown option character `\\x%x'.\n",
                        optopt);
            usage();
            return 1;
        }
        default:
            exit(1);
        }
    }

    printf("mu = %u, precision = %f, tau = %u, delta = %u, window_size = %u, seed = %lu\n",
           mu, precision, tau, delta, window_size, seed);

    printf("points_per_cluster = %u, number_of_clusters = %u, periods = %u, max_lng = %f, min_lng = %f, max_lat = %f, min_lat = %f, min_dist = %f\n",
           points_per_cluster, number_of_clusters, periods, max_lng, min_lng, max_lat, min_lat, min_dist);

    for (int index = optind; index < argc; index++)
    {
        fprintf(stderr, "Non-option argument %s\n", argv[index]);
    }

    file_entries = generate_data(points_per_cluster,
                                 number_of_clusters,
                                 periods,
                                 seed,
                                 max_lng,
                                 min_lng,
                                 max_lat,
                                 min_lat,
                                 min_dist);

    printf("Generated %lu entries\n", file_entries.size());

    int current_time = -1; //

    key_set significant_tiles;
    key_map total;
    sliding_window window;
    std::vector<cluster_point> clusters;

    if (timer_flag)
    {
        start_time = std::chrono::high_resolution_clock::now();
    }

    // leggo il dataset
    for (const auto &entry : file_entries)
    {
        auto row_time = entry.time;
        auto row_x = entry.x;
        auto row_y = entry.y;

        point k = {surject(row_x, precision), surject(row_y, precision)};
        if ((int)row_time > current_time)
        {
            next_period(clusters, current_time, significant_tiles, mu, precision, delta);
            current_time = row_time;

            #ifdef DEBUG
                printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
                printf("DEBUG: Processing time %d\n", current_time);
                printf("DEBUG: sigma size %ld\n", significant_tiles.size());
            #endif
            
            int time_key = current_time - window_size;

            // 1 is present, 0 is not present
            if (window.count(time_key) == 1)
            {

                #ifdef DEBUG
                    printf("DEBUG: Removing time %d\n", time_key);
                    printf("DEBUG: non dovrei essere qui: window.count(time_key) > 0\n");
                #endif

                key_map vals = window[time_key];
                key_map::iterator it;

                for (it = vals.begin(); it != vals.end(); it++)
                {
                    point c = it->first; // prendo la chiave di vals
                    int old_count = total[c];
                    total[c] -= it->second; // prendo il valore di vals
                    int new_count = total[c];
                    // TODO : da controllare
                    if (new_count <= 0)
                    {
                        // total[c] = 0;
                        total.erase(c);
                    }
                    if (old_count >= tau && new_count < tau)
                    {
                        significant_tiles.erase(c);
                    }
                }
                window.erase(time_key);
            }
        }

        total[k] = total[k] + 1;
        window[current_time][k] = window[current_time][k] + 1;

        if (total[k] == tau)
        {
            significant_tiles.insert(k);
        }

    }

    if (timer_flag)
    {
        // Calculate elapsed time and print
        auto end_time = std::chrono::high_resolution_clock::now(); // End time
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        printf("Elapsed time: %ld milliseconds\n", duration.count());
        printf("Operations per second: %f\n", (double)file_entries.size() / duration.count() * 1000);
    }

    FILE *fp = fopen("cluster.csv", "w");
    if (fp == NULL)
    {
        perror("Error opening file");
        return 1;
    }
    for (cluster_point &cluster : clusters)
    {
        fprintf(fp, "%f,%f,%d,%d\n", cluster.x, cluster.y, cluster.time, cluster.cluster_id);
    }
    fclose(fp);

    fp = fopen("generated_data.csv", "w");
    if (fp == NULL)
    {
        perror("Error opening file");
        return 1;
    }
    uint32_t cluster_id = 0;
    uint32_t row_count = 0;
    for (file_entry &entry : file_entries)
    {
        if (row_count++ % points_per_cluster == 0)
        {
            cluster_id++;
        }
        fprintf(fp, "%f,%f,%d,%u\n", entry.x, entry.y, entry.time, cluster_id);
    }

    return 0;
}
