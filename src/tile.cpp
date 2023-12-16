#include "tile.h"

int surject(double x, int precision){
    return (int)(pow(10, precision) * x);
}

double inverse_surject(int x, int precision){
    return (double)x / pow(10, precision);
}