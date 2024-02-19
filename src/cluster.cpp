#include "cluster.h"

void calculate_results(std::vector<cluster_point>& results, key_set& significant_tiles, int delta, int mu, float precision, int current_time)
{
            std::vector<std::unordered_set<point>> clusters;

            // https://thispointer.com/modify-elements-while-iterating-over-a-set-in-c/
            while (!significant_tiles.empty()) // sigma not empty
            {
                std::unordered_set<point> visit;
                std::unordered_set<point> cluster;
                try
                {
                    auto sigma_it = significant_tiles.begin();
                    point p_s = *sigma_it;
                    visit.insert(p_s);
                    significant_tiles.erase(p_s);
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << '\n';
                }

                while (!visit.empty())
                {
                    auto it = visit.begin();
                    point p_v = *it;

                    try
                    {
                        visit.erase(p_v);
                        std::vector<point> ns = neighborhood(significant_tiles, &p_v, delta);
                        cluster.insert(p_v);

                        for (const point element : ns)
                        {
                            significant_tiles.erase(element);
                            visit.insert(element);
                        }
                    }
                    catch (const std::exception &e)
                    {
                        std::cerr << e.what() << '\n';
                    }
                }
                if ((int)cluster.size() >= mu)
                {
                    try
                    {
                        clusters.push_back(cluster);
                    }
                    catch (const std::exception &e)
                    {
                        std::cerr << e.what() << '\n';
                    }
                }
            }

            uint16_t id = 0;   
    
            for (std::unordered_set<point> kn : clusters)
            {
                for (point k : kn)
                {
                    double rescaled_x = inverse_surject(k.x, precision);
                    double rescaled_y = inverse_surject(k.y, precision);
                    cluster_point cp = {rescaled_x, rescaled_y, current_time, id};
                    results.push_back(cp);
                }
                id++;
            }    
}

// io ho inteso adiacenza e non adiacenza cardinale https://freeciv.fandom.com/wiki/Adjacency#:~:text=Adjacency%20is%20when%20a%20tile,referred%20to%20as%20CARDINALLY%20ADJACENT.
std::vector<point> neighborhood(key_set sigma, const point *center, uint8_t delta)
{
    std::vector<point> kl;
    for (int i = center->x - delta; i <= center->x + delta; i++)
    {
        for (int j = center->y - delta; j <= center->y + delta; j++)
        {
            if (i == center->x && j == center->y)
                continue; // skippa alla prossima operazione
            point k = {i, j};

            // find return end if not found
            if (sigma.find(k) != sigma.end())
            {
                kl.push_back(k);
            }
        }
    }
    return kl;
}