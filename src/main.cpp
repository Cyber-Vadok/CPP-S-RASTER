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
#include <climits>

// my include
#include "tile.h"
#include "cluster.h"
#include "utils.h"
#include "accumulation.h"

#define MAX_PRECISION 14.0 // massima precisione per i valori generati

#define MAX_FILE_PATH_LENGTH 256 // Adjust the maximum length as needed

// global variables

FILE *fp;

// TODO: il numero massimo di significant tile vicine dipende da delta

// parametri algoritmo trovati nel paper
unsigned int mu = 4;        // Minimum cluster size in terms of the number of signifcant tiles
float precision = 3.5; // Precision for projection operation anche float
unsigned int tau = 5;       // Threshold number of points to determine if a tile is signifcant
unsigned int delta = 1;     // Distance metric for cluster defnition
unsigned int window_size = 10;

// parametri implementazione
std::string file_path_input;
std::string file_path_output;
std::string file_path_time;
std::string folderName;
bool timer_flag = false;  // stampa tempo di esecuzione
bool output_flag = false; // stampa su file
bool verbose_flag = false;
std::chrono::high_resolution_clock::time_point start_time;

void usage()
{
    fprintf(stderr, "Usage: ./sraster -f input-filepath [-c <time-filepath>] [-o <output-filepath>] [-m <mu>] [-p <xi>] [-t <tau>] [-d <delta>] [-w <window size>]\n");
    fprintf(stderr, "  -f filepath Set the input file path (mandatory)\n");
    fprintf(stderr, "  -o filepath Set the output file path\n");
    fprintf(stderr, "  -m <mu>  Set minimum cluster size in terms of the number of signifcant tiles (default: 4, max = %u)\n", UINT_MAX);
    fprintf(stderr, "  -p <xi>  Set precision for projection operation (default: 3.5, max = %f)\n", MAX_PRECISION);
    fprintf(stderr, "  -t <tau> Set threshold number of points to determine if a tile is significant (default: 5, max = %u)\n", UINT_MAX);
    fprintf(stderr, "  -d <delta> Set distance metric for cluster definition (default: 1, max = %u)\n", UINT_MAX);
    fprintf(stderr, "  -w <window size> Set window size (default: 10)\n");
    fprintf(stderr, "  -c <time-filepath> Print execution time and set file path where time info will be saved\n");
}

int main(int argc, char **argv)
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // argument parser
    // https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html
    int c;
    while ((c = getopt(argc, argv, "c:f:o:m:p:t:d:w:s:hv")) != -1)
    {
        switch (c)
        {
        case 'h': // help
        {
            usage();
            exit(1);
            break;
        }
        case 'm': // minimum cluster size
        {
            if (std::stoul(optarg) > UINT_MAX)
            {
                fprintf(stderr, "Invalid mu value, max = %u\n", UINT_MAX);
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
            if (std::stoul(optarg) > UINT_MAX)
            {
                fprintf(stderr, "Invalid tau value, max = %u\n", UINT_MAX);
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
            if (std::stoul(optarg) > UINT_MAX)
            {
                fprintf(stderr, "Invalid delta value, max = %u\n", UINT_MAX);
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
            if (std::stoul(optarg) > UINT_MAX)
            {
                fprintf(stderr, "Invalid window size value, max = %u\n", UINT_MAX);
                usage();
                exit(EXIT_FAILURE);
            }
            else
            {
                window_size = std::stoul(optarg);
            }
            break;
        }
        case 'c': // ouatput tempo
        {
            if (strlen(optarg) >= MAX_FILE_PATH_LENGTH)
            {
                fprintf(stderr, "Error: File path exceeds maximum length\n");
                exit(EXIT_FAILURE);
            }
            if (std::filesystem::is_directory(optarg))
            {
                fprintf(stderr, "Error: Output file path is a directory\n");
                exit(EXIT_FAILURE);
            }
            file_path_time = optarg;
            timer_flag = true;
            break;
        }
        case 'f': // input
            if (strlen(optarg) >= MAX_FILE_PATH_LENGTH)
            {
                fprintf(stderr, "Error: File path exceeds maximum length\n");
                exit(EXIT_FAILURE);
            }
            file_path_input = optarg;
            break;

        case 'o': // output cluster

            if (strlen(optarg) >= MAX_FILE_PATH_LENGTH)
            {
                fprintf(stderr, "Error: File path exceeds maximum length\n");
                exit(EXIT_FAILURE);
            }
            if (std::filesystem::is_directory(optarg))
            {
                fprintf(stderr, "Error: Output file path is a directory\n");
                exit(EXIT_FAILURE);
            }
                file_path_output = optarg;
            output_flag = true;
            break;

        case 'v':
            verbose_flag = true;
            break;

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

    if (file_path_input.empty())
    {
        fprintf(stderr, "Missing file input path\n");
        usage();
        exit(EXIT_FAILURE);
    }

    printf("mu = %u, precision = %f, tau = %u, delta = %u, window_size = %u,",
           mu, precision, tau, delta, window_size);

    printf(" file_path_input = %s\n", file_path_input.c_str());

    for (int index = optind; index < argc; ++index)
    {
        std::cout << argv[index] << std::endl;
    }

    // printf("points_per_cluster = %u, number_of_clusters = %u, periods = %u, max_lng = %f, min_lng = %f, max_lat = %f, min_lat = %f, min_dist = %f\n",
    //        points_per_cluster, number_of_clusters, periods, max_lng, min_lng, max_lat, min_lat, min_dist);

    for (int index = optind; index < argc; index++)
    {
        fprintf(stderr, "Non-option argument %s\n", argv[index]);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // carico i dati
    std::vector<file_entry> file_entries = read_data(file_path_input);

    file_entries.push_back({0.0, 0.0, 10}); // TO DO : stop point a che serviva?

    int current_time = -1;

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
            //DEBUG : bloccato a processing time 1
            // printf("Processing time %d\n", row_time);

            calculate_results(results, significant_tiles, delta, mu, precision, current_time); // send 0

            current_time = row_time;
            int time_key = current_time - window_size;

            // 1 is present, 0 is not present
            if (window.count(time_key) == 1)
            {
                slide_window(window, total, significant_tiles, time_key, tau); // eventually send -1
            }
        }

        // update count
        total[k] = total[k] + 1;
        window[current_time][k] = window[current_time][k] + 1;

        if (total[k] == tau)
        {
            significant_tiles.insert(k); // send 1
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

        // Open or create the time file
        if (!std::filesystem::exists(file_path_time))
        {
            std::ofstream timeFile(file_path_time); // Open file in append mode
            // If the file doesn't exist, create it and write headers
            if (timeFile.is_open())
            {
                timeFile << "Elapsed Time (ms),Operations per second\n";
            }
            else
            {
                std::cerr << "Error creating time file" << std::endl;
                return 1; // Exit function or handle error as appropriate
            }
        }

        std::ofstream timeFile(file_path_time, std::ios::app); // Open file in append mode
        timeFile << duration.count() << "," << (long double)file_entries.size() / duration.count() * 1000 << "\n";
        timeFile.close();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if (output_flag)
    {
        std::ofstream outputFile(file_path_output);
        if (!outputFile.is_open())
        {
            std::cerr << "Error opening file" << std::endl;
            return 1;
        }

        outputFile << "x,y,time,cluster_id\n";
        // Write data to the file
        for (const cluster_point &cluster : results)
        {
            outputFile << cluster.x << "," << cluster.y << "," << cluster.time << "," << cluster.cluster_id << "\n";
        }
    }
    // else
    // {
    //     printf("clusters found: %d\n", cluster_id);
    // }

    return 0;
}
