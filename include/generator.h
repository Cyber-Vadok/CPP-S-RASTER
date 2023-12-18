#ifndef GENERATOR_H
#define GENERATOR_H

#include <cstdint>
#include <random>

typedef struct file_entry
{
    double x;
    double y;
    uint32_t time;
} file_entry;

std::vector<file_entry> generate_data(
                    uint32_t points_per_cluster,
                    uint32_t number_of_clusters, // per period
                    uint32_t periods,
                    uint64_t seed,
                    double max_lng,
                    double min_lng,
                    double max_lat,
                    double min_lat,
                    double min_dist);

#endif // GENERATOR_H