#include "cluster.h"

std::vector<std::vector<point>> cluster(key_set sigma, uint8_t mu, uint8_t delta)
{
    std::vector<std::vector<point>> clusters;

    // https://thispointer.com/modify-elements-while-iterating-over-a-set-in-c/
    while (!sigma.empty()) // sigma not empty
    {
        std::unordered_set<point> visit;
        std::vector<point> cluster;
        try
        {
            auto sigma_it = sigma.begin();
            point p_s = *sigma_it;
            visit.insert(p_s);
            sigma.erase(p_s);
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
                std::vector<point> ns = neighborhood(sigma, &p_v, delta);
                cluster.push_back(p_v);

                for (const point element : ns)
                {
                    sigma.erase(element);
                    visit.insert(element);
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
            }

            if (cluster.size() >= mu)
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
    }
    return clusters;
}

// ricorda che ha senso che se tutti i punti sono vicini tra loro vadano tutti nello stesso cluster con id = 0
void next_period(std::vector<cluster_point> &temp, uint32_t time, key_set sigma, uint8_t mu, float precision, uint8_t delta)
{
    uint16_t id = 0;
    std::vector<std::vector<point>> knl = cluster(sigma, mu, delta);
   
    if (knl.empty())
    {
        return; 
    }

    for (std::vector<point> kn : knl)
    {
        for (point k : kn)
        {
            double rescaled_x = inverse_surject(k.x, precision);
            double rescaled_y = inverse_surject(k.y, precision);
            cluster_point cp = {rescaled_x, rescaled_y, time, id};
            temp.push_back(cp);
            // printf("%f %f %d %d\n", cp.x, cp.y, cp.time, cp.cluster_id);
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
            if (sigma.find(k) != sigma.end())
            {
                kl.push_back(k);
            }
        }
    }
    return kl;
}