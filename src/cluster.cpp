#include "cluster.h"

std::vector<std::vector<point>> cluster(key_set sigma, uint8_t mu, uint8_t delta)
{
    std::vector<std::vector<point>> clusters;
    std::unordered_set<point> visit;

    // tile_set *visit = create_tile_set(100); // TODO: l'ho messo qui perche' non serve crearla ogni volta
    // https://thispointer.com/modify-elements-while-iterating-over-a-set-in-c/
    while (!sigma.empty()) // sigma not empty
    {
        std::vector<point> cluster;
        try
        {
            auto sigma_it = sigma.begin();
            visit.insert(*sigma_it);
            sigma.erase(*sigma_it);
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }

        while (!visit.empty())
        {
            auto it = visit.begin();

            try
            {
                visit.erase(*it);
                std::vector<point> ns = neighborhood(sigma, &(*it), delta);
                cluster.push_back(*it);

                for (const point &element : ns)
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
void next_period(int time, key_set sigma, uint8_t mu, uint8_t precision, uint8_t delta)
{
    uint16_t id = 0;

    FILE *file = fopen("output.txt", "a"); // Open a file for appending

    // Check if the file is opened successfully
    if (file == NULL)
    {
        fprintf(stderr, "Error opening file.\n");
        // Handle the error as needed
        return;
    }

    fprintf(file, "==========================time:%d==========================\n", time);

    std::vector<std::vector<point>> knl = cluster(sigma, mu, delta);

    if (knl.empty())
    {
        fprintf(file, "No clusters found\n");
        fclose(file);
        return;
    }

    for (std::vector<point> kn : knl)
    {
        for (point k : kn)
        {
            double rescaled_x = inverse_surject(k.x, precision);
            double rescaled_y = inverse_surject(k.y, precision);
            fprintf(file, "|   time : %-12d   |   id : %-12d   |   x : %-18.6lf   |   y : %-18.6lf   |\n", time, id, rescaled_x, rescaled_y);
        }
        id++;
    }

    fclose(file);
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