#include "tile.h"

int surject(double x, float precision){
    return (int)(pow(10, precision) * x);
}

double inverse_surject(int x, float precision){
    return (double)x / pow(10, precision);
}