#ifndef CLUSTER_H
#define CLUSTER_H

#include "tile.h"

#include <vector>
#include <iostream>

typedef struct cluster_point
{
    double x;
    double y;
    int time;
    unsigned int cluster_id;
} cluster_point;

std::vector<point> neighborhood (key_set sigma, const point * center, unsigned int delta);
// std::vector<std::vector<point>> cluster (key_set sigma, unsigned int mu, unsigned int delta);
void next_period(std::vector<cluster_point> &temp, int time, key_set sigma, unsigned int mu, float precision, unsigned int delta);
void calculate_results(std::vector<cluster_point>& results, key_set& significant_tiles, int delta, int mu, float precision, int current_time);

#endif