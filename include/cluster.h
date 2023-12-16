#ifndef CLUSTER_H
#define CLUSTER_H

#include "xxhash.h"

#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "tile.h"

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iostream>

std::vector<point> neighborhood (key_set sigma, const point * center, uint8_t delta);

int rescale(double x, int precision);

std::vector<std::vector<point>> cluster (key_set sigma, uint8_t mu, uint8_t delta);
void next_period(int time, key_set sigma, uint8_t mu, uint8_t precision, uint8_t delta);

#endif