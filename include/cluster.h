#ifndef CLUSTER_H
#define CLUSTER_H

#include "tile.h"

#include <vector>
#include <iostream>

typedef struct cluster_point
{
    double x;
    double y;
    uint32_t time;
    uint32_t cluster_id;
} cluster_point;

std::vector<point> neighborhood (key_set sigma, const point * center, uint8_t delta);
std::vector<std::vector<point>> cluster (key_set sigma, uint8_t mu, uint8_t delta);
void next_period(std::vector<cluster_point> &temp, uint32_t time, key_set sigma, uint8_t mu, float precision, uint8_t delta);

#endif