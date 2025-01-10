#!/bin/bash

seed=$(date +%s)

# Function to show usage instructions
show_help() {
  echo "Usage: $0 [-w value] [-n value] [-p value] [-f filename]"
}

# Default values
w=10
n=10
p=50
filename="output_$seed.csv"

# Parse command line arguments
while getopts "w:n:p:f:h" opt; do
  case $opt in
    w)
      w=$OPTARG
      ;;
    n)
      n=$OPTARG
      ;;
    p)
      p=$OPTARG
      ;;
    f)
      filename=$OPTARG
      ;;
    h)
      show_help
      exit 0
      ;;
    *)
      show_help
      exit 1
      ;;
  esac
done

# Validate inputs, checks if a integer positive number
if [[ ! $w =~ ^[0-9]+$ ]] || [[ ! $n =~ ^[0-9]+$ ]] || [[ ! $p =~ ^[0-9]+$ ]] || [[ ! $filename =~ ^[a-zA-Z0-9._-]+$ ]]; then
  echo "Invalid input. Please check the arguments."
  show_help
  exit 1
fi

# Generate data
seed=$(date +%s)
total_points=$(($p * $n))
points_per_day=$(($total_points / $w))

# Make sure the path exists
path="input"
mkdir -p $path

# Run the Python scripts
python3 data_generator/gen.py -n $n -p $p -s $seed -o "${path}/${filename}"
python3 data_generator/change_label.py "${path}/${filename}" $points_per_day

echo "Data generation complete. Output saved to ${path}/${filename}"
