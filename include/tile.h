#ifndef TILE_H
#define TILE_H

#include <math.h>

#include "xxhash.h"

#include <cstdint>
#include <unordered_set>
#include <unordered_map>

typedef struct point {
    int x;
    int y;

    bool operator==(const point &other) const{
        return x == other.x && y == other.y;
    }
} point;

namespace std{
    template<>
    struct hash<point>{
        size_t operator()(const point &k) const{
            return XXH64(&k, sizeof(point), 0);
        }
    };
}

typedef std::unordered_set<point> key_set;
typedef std::unordered_map<point, uint32_t> key_map;
typedef std::unordered_map<uint32_t, key_map> sliding_window;

int surject(double x, int precision);
double inverse_surject(int x, int precision);

#endif  