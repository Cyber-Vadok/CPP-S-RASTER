#ifndef TILE_H
#define TILE_H

#include <math.h>

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

//http://szudzik.com/ElegantPairing.pdf
namespace std{
    template<>
    struct hash<point>{
        size_t operator()(const point &k) const{
            int a = k.x >= 0 ? 2 * k.x : -2 * k.x - 1;
            int b = k.y >= 0 ? 2 * k.y : -2 * k.y - 1;
            int comb = a >= b ? a * a + a + b : a + b * b;
            return hash<int>()(comb);
        }
    };
}

typedef std::unordered_set<point> key_set;
typedef std::unordered_map<point, uint32_t> key_map;
typedef std::unordered_map<uint32_t, key_map> sliding_window;

int surject(double x, float precision);
double inverse_surject(int x, float precision);

#endif  