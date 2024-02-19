#!/bin/bash

# Check if the folder data_input exists
folder="data_input"
if [ ! -d "$folder" ]; then
    # If the folder doesn't exist, create it
    mkdir "$folder"
fi

# List of files to check
files=("data_1.csv" "data_10.csv" "data_100.csv" "data_1000.csv" "data_10000.csv")

# Flag to indicate whether any file is missing
missing_file=false

# Check for the existence of each file
for file in "${files[@]}"; do
    if [ ! -e "$folder/$file" ]; then
        echo "File $file is missing."
        missing_file=true
    fi
done

# If any file is missing, run the Python script
if [ "$missing_file" = true ]; then
    python3 data_generator/generator_big.py "$folder"
else
    echo "All required files exist."
fi

# Iterate over each CSV file in the folder and add time label
for csv_file in "$folder"/*.csv; do
    # Check if the file is a regular file
    if [ -f "$csv_file" ]; then
        # Extract the number from the filename and calculate points
        # number=$(basename "$filename" .csv | awk -F '_' '{print $2}')
        number=$(basename "$csv_file" .csv | sed 's/[^0-9]*//g')
        points=$((number * 50))

        # Run the Python script with the CSV file and calculated points as arguments
        python3 data_generator/change_label.py "$csv_file" "$points"
        # echo "File $csv_file is a regular file."
    fi
done