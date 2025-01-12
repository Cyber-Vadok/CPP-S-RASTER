#!/bin/bash

# Function to show usage instructions
show_help() {
  echo "Usage: $0 -w value -n value -p value -f filename"
  echo "  -w value   Number of days (positive integer)"
  echo "  -n value   Number of clusters (positive integer)"
  echo "  -p value   Points per cluster (positive integer)"
  echo "  -f filename Output filename (valid file name)"
}

# Initialize variables
w=""
n=""
p=""
filename=""

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

# Check if all required parameters are provided
if [[ -z $w || -z $n || -z $p || -z $filename ]]; then
  echo "Error: Missing required parameters."
  show_help
  exit 1
fi

# Validate inputs (checks for positive integers and valid filename)
if [[ ! $w =~ ^[0-9]+$ ]] || [[ ! $n =~ ^[0-9]+$ ]] || [[ ! $p =~ ^[0-9]+$ ]] || [[ ! $filename =~ ^[a-zA-Z0-9._-]+$ ]]; then
  echo "Error: Invalid input. Please check the arguments."
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
