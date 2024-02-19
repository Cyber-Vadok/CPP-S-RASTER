#include "utils.h"

std::vector<file_entry> read_data(std::string filename) {
    std::vector<file_entry> data;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open file " << filename << std::endl;
        return data;
    }
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        file_entry entry;
        char delimiter; // Variable to store the comma separator

        // Set maximum precision for reading double values
        iss >> std::setprecision(std::numeric_limits<double>::max_digits10);

        if (!(iss >> entry.x >> delimiter >> entry.y >> delimiter >> entry.time)) {
            // Failed to read all fields, handle the error
            std::cerr << "Failed to read line: " << line << std::endl;
            continue; // Skip to the next line
        }
        data.push_back(entry);
    }
    file.close();
    return data;
}

// Function to get the current timestamp as a string
std::string getCurrentTimestamp() {
    char buffer[80];
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%Y%m%d%H%M%S", timeinfo);

    return std::string(buffer);
}