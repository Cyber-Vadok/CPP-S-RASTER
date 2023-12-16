// Created by Roberto Vadacca.

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
#include <vector>
#include <chrono>

// my include
#include "tile.h"
#include "cluster.h"

// definitions

typedef struct file_entry
{
    int time;
    double x;
    double y;
} file_entry;

// global variables

std::vector<file_entry> file_entries;

// parametri algoritmo
uint8_t mu = 4;        // Minimum cluster size in terms of the number of signifcant tiles
uint8_t precision = 3; // Precision for projection operation
uint8_t tau = 5;       // Threshold number of points to determine if a tile is signifcant
uint8_t delta = 1;     // Distance metric for cluster defnition
uint8_t window_size = 10;

// parametri implementazione
bool timer_flag = false; // stampa tempo di esecuzione

std::chrono::high_resolution_clock::time_point start_time;

void usage()
{
    fprintf(stderr, "Usage: ./sraster [-m <mu>] [-p <xi>] [-t <tau>] [-d <delta>] [-w <window size>]\n");
    fprintf(stderr, "  -m <mu>  Set minimum cluster size in terms of the number of signifcant tiles (default: 5, max = %u)\n", UINT8_MAX);
    fprintf(stderr, "  -p <xi>  Set precision for projection operation (default: 5, max = %u)\n", UINT8_MAX);
    fprintf(stderr, "  -t <tau> Set threshold number of points to determine if a tile is significant (default: 8, max = %u)\n", UINT8_MAX);
    fprintf(stderr, "  -d <delta> Set distance metric for cluster definition (default: 1, max = %u)\n", UINT8_MAX);
    fprintf(stderr, "  -w <window size> Set window size (default: 5)\n");
    // fprintf(stderr, "  -f <file>         Set output file (default: ***)\n");
    // fprintf(stderr, "  -v                Verbose output\n");
    // fprintf(stderr, "  -t                Print elapsed time and operations per second\n");
    // fprintf(stderr, "  -h                Print this help message\n");
    // fprintf(stderr, "  -l                Set the length of the array\n");
}

int main(int argc, char **argv)
{

    // long maxFileLength = pathconf("/", _PC_PATH_MAX); // POSIX
    // if (maxFileLength == -1) {
    //     // An error occurred while determining the maximum file length
    //     perror("Error getting maximum file length");
    //     return 1;
    // }

    // uint32_t lenght_value = 0;
    // uint32_t convergence_value = 0;
    // std::string filename = "";
    // bool verbose_flag = false;
    // bool time_flag = false;
    // int index;
    // int c; // getopt() character

    opterr = 0;

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
            if (std::stoul(optarg) > UINT8_MAX)
            {
                fprintf(stderr, "Invalid mu value, max = %u\n", UINT8_MAX);
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
            if (std::stoul(optarg) > UINT8_MAX)
            {
                fprintf(stderr, "Invalid precision value, max = %u\n", UINT8_MAX);
                usage();
                exit(EXIT_FAILURE);
            }
            else
            {
                precision = std::stoul(optarg);
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

        printf("mu = %u, precision = %u, tau = %u, delta = %u\n",
               mu, precision, tau, delta);
        // printf("convergence_value = %u, lenght_value = %u, filename = %s, time = %d, verbose = %d\n",
        //        convergence_value, lenght_value, filename.c_str(), time_flag, verbose_flag);

        for (int index = optind; index < argc; index++)
        {
            fprintf(stderr, "Non-option argument %s\n", argv[index]);
        }
    }

    // read and store dataset
    int row_time;
    double row_x;
    double row_y;
    uint32_t row_count = 0;

    FILE *fp = fopen("/home/robivad/Git/MyS-Raster/data_generator/timed_data_10000.csv", "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        return 1;
    }
    printf("loading file into memory\n");
    while (fscanf(fp, "%lf,%lf,%d", &row_x, &row_y, &row_time) == 3)
    {
        file_entry entry = {row_time, row_x, row_y};
        try
        {
            file_entries.push_back(entry);
            row_count++;
        }
        catch (const std::bad_alloc &e)
        {
            std::cerr << "Error allocating memory: " << e.what() << std::endl;
            return 1;
        }
    }
    printf("\n reading complete\n");

    int current_time = INT32_MIN; //

    key_set significant_tiles;
    key_map total;
    sliding_window window;

    if (timer_flag)
    {
        start_time = std::chrono::high_resolution_clock::now();
    }

    int count = 0;
    // leggo il dataset
    for (const auto &entry : file_entries)
    {
        row_time = entry.time;
        row_x = entry.x;
        row_y = entry.y;

        if (count++ % 50000 == 0)
        {
            printf("count = %d\n", count);
        }

        point k = {surject(row_x, precision), surject(row_y, precision)};
        if (row_time > current_time)
        {
            next_period(current_time, significant_tiles, mu, precision, delta);
            current_time = row_time;
            int time_key = current_time - window_size;

            printf("current_time = %d\n", current_time);


            // da grandi poteri grandi responsabilità

            // TODO : fix this shit
            if (window.count(time_key) > 0)
            {
                key_map vals = window[time_key];
                key_map::iterator it;

                for (it = vals.begin(); it != vals.end(); it++)
                {
                    point c = it->first; // prendo la chiave di vals
                    int old_count = total[c];
                    total[c] -= it->second; // prendo il valore di vals
                    int new_count = total[c];
                    // TODO : da controllare
                    if (new_count < 0)
                    {
                        total[c] = 0;
                    }
                    if (old_count >= tau && new_count < tau)
                    {
                        significant_tiles.erase(c);
                    }

                    // new count non potrebbe anche diventare negativo?
                    if (new_count == 0) // se la tile è vuota
                    {
                        total.erase(c);
                    }
                }
                window.erase(time_key);
            }
        }

        total[k] = total[k] + 1;
        window[current_time][k] = window[current_time][k] + 1;

        // basta inserire sola la prima volta, una volta, non mille
        if (total[k] == tau)
        {
            significant_tiles.insert(k);
        }
    }

    printf("ho finito di leggere/n");

    if (timer_flag)
    {
        // Calculate elapsed time and print
        auto end_time = std::chrono::high_resolution_clock::now(); // End time
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        printf("Elapsed time: %ld milliseconds\n", duration.count());
        printf("Operations per second: %f\n", (double)file_entries.size() / duration.count() * 1000);
    }

    fclose(fp);
    return 0;
}
