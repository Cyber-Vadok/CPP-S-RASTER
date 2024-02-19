#include "tile.h"

int surject(double x, float precision){
    return (int)(pow(10, precision) * x);
}

double inverse_surject(int x, float precision){
    return (double)x / pow(10, precision);
}

void printKeySet(const key_set& set) {
    std::cout << "Key Set Elements:" << std::endl;
    for (const auto& p : set) {
        std::cout << "(" << p.x << ", " << p.y << ")" << std::endl;
    }
}

void printPointVector(const std::vector<point>& vec) {
    std::cout << "Point Vector Elements:" << std::endl;
    for (const auto& p : vec) {
        std::cout << "(" << p.x << ", " << p.y << ")" << std::endl;
    }
}

void printPoint(const point& p) {
    std::cout << "(" << p.x << ", " << p.y << ")" << std::endl;
}