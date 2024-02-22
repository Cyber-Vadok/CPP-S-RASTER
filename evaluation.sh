#!/bin/bash

# Check if the input_folder data_input exists
input_folder="data_input"
if [ ! -d "$input_folder" ]; then
    # If the input_folder doesn't exist, create it
    mkdir "$input_folder"
fi

output_folder="data_output"
if [ ! -d "$output_folder" ]; then
    # If the input_folder doesn't exist, create it
    mkdir "$output_folder"
fi

time_folder="time_output"
if [ ! -d "$time_folder" ]; then
    # If the input_folder doesn't exist, create it
    mkdir "$time_folder"
fi

# List of files to check
files=("data_1.csv" "data_10.csv" "data_100.csv" "data_1000.csv" "data_10000.csv")

# Flag to indicate whether any file is missing
missing_file=false

# Check for the existence of each file
for file in "${files[@]}"; do
    if [ ! -e "$input_folder/$file" ]; then
        echo "File $file is missing."
        missing_file=true
    fi
done

# If any file is missing, run the Python script
if [ "$missing_file" = true ]; then
    python3 data_generator/generator_big.py "$input_folder"
else
    echo "All required files exist."
fi

# Iterate over each CSV file in the input_folder and add time label
for csv_file in "$input_folder"/*.csv; do
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

# run the C program to evaluate the data
make all
if [ $? -ne 0 ]; then
    echo "Error building the C program."
    exit 1
fi

timestamp=$(date +"%Y-%m-%d_%H-%M-%S")

# cluster evaluation
for csv_file in "$input_folder"/*.csv; do
    ./build/bin/sraster -f "$csv_file" -o "$output_folder/$(basename $csv_file)_$timestamp.csv"
    for i in {1..10}; do
        ./build/bin/sraster -f "$csv_file" -c "$time_folder/$(basename $csv_file)_$timestamp.csv"
    done
    if [ $? -ne 0 ]; then
        echo "Error running the C program."
        exit 1
    fi
done

# graphs
python3 time_output/plot.py