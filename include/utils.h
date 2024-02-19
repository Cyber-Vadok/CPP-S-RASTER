#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include "tile.h"

typedef struct file_entry
{
    double x;
    double y;
    int time;
} file_entry;

std::vector<file_entry> read_data(std::string filename);
std::string getCurrentTimestamp();

#endif