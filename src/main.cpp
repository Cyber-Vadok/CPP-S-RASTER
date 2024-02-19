// Created by Roberto Vadacca.

// c include
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// c++ include
#include <cstdint>
#include <string>
#include <chrono>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <filesystem> // for filesystem operations

// my include
#include "tile.h"
#include "cluster.h"
#include "generator.h"
#include "utils.h"
#include "accumulation.h"

#define MAX_PRECISION 14.0 // massima precisione per i valori generati
#define MAX_MU 8           // massimo numero di vicini per una tile

// global variables

FILE *fp;

// parametri algoritmo trovati nel paper
uint8_t mu = 4;        // Minimum cluster size in terms of the number of signifcant tiles
float precision = 3.5; // Precision for projection operation anche float
uint8_t tau = 5;       // Threshold number of points to determine if a tile is signifcant
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
std::string file_path;
bool timer_flag = false; // stampa tempo di esecuzione
std::chrono::high_resolution_clock::time_point start_time;

void usage()
{
    fprintf(stderr, "Usage: ./sraster [-m <mu>] [-p <xi>] [-t <tau>] [-d <delta>] [-w <window size>]\n");
    fprintf(stderr, "  -m <mu>  Set minimum cluster size in terms of the number of signifcant tiles (default: 4, max = %u)\n", UINT8_MAX);
    fprintf(stderr, "  -p <xi>  Set precision for projection operation (default: 3.5, max = %f)\n", MAX_PRECISION);
    fprintf(stderr, "  -t <tau> Set threshold number of points to determine if a tile is significant (default: 5, max = %u)\n", UINT8_MAX);
    fprintf(stderr, "  -d <delta> Set distance metric for cluster definition (default: 1, max = %u)\n", UINT8_MAX);
    fprintf(stderr, "  -w <window size> Set window size (default: 10)\n");
    fprintf(stderr, "  -f filepath\n"); //TODO: trovare un nome migliore
    fprintf(stderr, "  -s <seed> Set seed for random generator (default: 0)\n");
}

int main(int argc, char **argv)
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
        case 'f':
        {
            file_path = optarg;
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

    // printf("points_per_cluster = %u, number_of_clusters = %u, periods = %u, max_lng = %f, min_lng = %f, max_lat = %f, min_lat = %f, min_dist = %f\n",
    //        points_per_cluster, number_of_clusters, periods, max_lng, min_lng, max_lat, min_lat, min_dist);

    for (int index = optind; index < argc; index++)
    {
        fprintf(stderr, "Non-option argument %s\n", argv[index]);
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    // carico i dati
    std::vector<file_entry> file_entries = read_data(file_path);
    file_entries.push_back({0.0, 0.0, 10}); // stop point

    int current_time = -1; //

    key_set significant_tiles;
    key_map total;
    sliding_window window;
    std::vector<cluster_point> results;

    if (timer_flag)
    {
        start_time = std::chrono::high_resolution_clock::now();
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // leggo il dataset
    for (const auto &entry : file_entries)
    {
        auto row_time = entry.time;
        point k = {surject(entry.x, precision), surject(entry.y, precision)};

        if ((int)row_time > current_time)
        {
            
            calculate_results(results, significant_tiles, mu, delta, precision, current_time); // send 0

            current_time = row_time;
            int time_key = current_time - window_size;

            // 1 is present, 0 is not present
            if (window.count(time_key) == 1)
            {
                slide_window(window, total, significant_tiles, time_key, tau); //eventually send -1
            }
        }

        // update count
        total[k] = total[k] + 1;
        window[current_time][k] = window[current_time][k] + 1;

        if (total[k] == tau)
        {
            significant_tiles.insert(k); //send 1
        }
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if (timer_flag)
    {
        // Calculate elapsed time and print
        auto end_time = std::chrono::high_resolution_clock::now(); // End time
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        printf("Elapsed time: %ld milliseconds\n", duration.count());
        printf("Operations per second: %f\n", (double)file_entries.size() / duration.count() * 1000);
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string folderName = "data_output";
    // Create the folder if it doesn't exist
    if (!std::filesystem::exists(folderName)) {
        std::filesystem::create_directory(folderName);
    }

    // Open the file
    std::string filename = folderName + "/cluster_" + getCurrentTimestamp() + ".csv";
    std::ofstream outputFile(filename);
    if (!outputFile.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return 1;
    }

    // Write data to the file
    for (const cluster_point &cluster : results) {
        outputFile << cluster.x << "," << cluster.y << "," << cluster.time << "," << cluster.cluster_id << "\n";
    }

    return 0;
}
