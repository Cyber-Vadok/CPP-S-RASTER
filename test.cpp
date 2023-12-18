#include "./include/generator.h"

int main()
{
    std::vector<file_entry> file_entries;

    // parametri generazione dataset
    uint32_t points_per_cluster = 500;
    uint32_t number_of_clusters = 100; // per period
    uint32_t periods = 10;
    uint64_t seed = 0;
    double max_lng = 180.0;
    double min_lng = -180.0;
    double max_lat = 90.0;
    double min_lat = -90.0;
    double min_dist = 0.0500; // 50 * 1.1m

    file_entries = generate_data(points_per_cluster,
                                 number_of_clusters,
                                 periods,
                                 seed,
                                 max_lng,
                                 min_lng,
                                 max_lat,
                                 min_lat,
                                 min_dist);

    FILE * fp = fopen("generated_data.csv", "w");
    if (fp == NULL)
    {
        perror("Error opening file");
        return 1;
    }
    for (file_entry &entry : file_entries)
    {
        fprintf(fp, "%f,%f,%d\n", entry.x, entry.y, entry.time);
    }

    return 0;
}