#include "generator.h"

// cpp implementation of https://github.com/FraunhoferChalmersCentre/s-raster data generator.

std::vector<file_entry> generate_data(
    uint32_t points_per_cluster,
    uint32_t number_of_clusters, // per period
    uint32_t periods,
    uint64_t seed,
    double max_lng,
    double min_lng,
    double max_lat,
    double min_lat,
    double min_dist)
{
    std::mt19937_64 generator(seed);
    std::uniform_real_distribution<double> lng_dist(min_lng, max_lng);
    std::uniform_real_distribution<double> lat_dist(min_lat, max_lat);
    std::uniform_real_distribution<double> z_dist(0.0, 0.0010); // magic numbers

    std::vector<file_entry> points;

    for (uint32_t time = 0; time < periods; time++)
    {

        std::vector<std::pair<double, double>> centers;
        double x = lng_dist(generator);
        double y = lat_dist(generator);
        centers.push_back(std::make_pair(x, y));

        while (centers.size() < number_of_clusters)
        {
            for (auto &centroid : centers)
            {
                if (centers.size() >= number_of_clusters)
                {
                    break;
                }
                x = lng_dist(generator);
                y = lat_dist(generator);
                if (std::abs(centroid.first - x) > min_dist && std::abs(centroid.second - y) > min_dist)
                {
                    centers.push_back(std::make_pair(x, y));
                }
            }
        }

        for (auto &centroid : centers)
        {
            for (uint32_t i = 0; i < points_per_cluster; i++)
            {
                double z = z_dist(generator); // magic number

                std::uniform_real_distribution<double> eps_dist(0.0, z);
                double eps = eps_dist(generator);

                std::uniform_real_distribution<double> lgn_dist_noisy(centroid.first - eps, centroid.first + eps);
                std::uniform_real_distribution<double> lat_dist_noisy(centroid.second - eps, centroid.second + eps);

                file_entry entry = {lgn_dist_noisy(generator), lat_dist_noisy(generator), time};
                points.push_back(entry);
            }
        }
    }

    return points;
}
