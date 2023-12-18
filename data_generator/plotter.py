#!/usr/bin/env python3

import sys
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib import cm
from itertools import cycle

def plot_scatter_with_time_and_id(csv_path):
    # Read data from CSV file without a header
    data = pd.read_csv(csv_path, header=None, names=['x', 'y', 'time', 'id'])

    # Select only the first 500 rows
    subset_data = data.iloc[:500]

    # Extract columns
    x = subset_data['x']
    y = subset_data['y']
    time = subset_data['time']
    ids = subset_data['id']

    # Create a dictionary to map unique IDs to markers
    id_to_marker = {unique_id: marker for unique_id, marker in zip(ids.unique(), cycle(['o', 's', '^', 'D', 'v', '<', '>', 'p', '*', 'h', '+', 'x', '|', '_']))}

    # Create a list of markers based on IDs
    markers = [id_to_marker[unique_id] for unique_id in ids]

    # Creating a scatter plot with colors based on the time column and different markers based on IDs
    scatter = plt.scatter(x, y, c=time, cmap=cm.viridis, marker='o', edgecolors='black', linewidths=0.5, s=50, alpha=0.8)

    # Set different markers for each point
    scatter.set_paths(markers)

    # Adding colorbar to indicate the mapping of time to colors
    cbar = plt.colorbar(scatter, label='Time')

    # Adding labels and title
    plt.xlabel('X-axis')
    plt.ylabel('Y-axis')
    plt.title('Scatter Plot of 2D Data with Time and ID Marker Mapping')

    # Display the plot
    plt.show()

if __name__ == "__main__":
    # Check if the correct number of arguments is provided
    if len(sys.argv) != 2:
        print("Usage: {} <csv_path>".format(sys.argv[0]))
        sys.exit(1)

    # Get the CSV path from the command-line arguments
    csv_path = sys.argv[1]

    # Call the function to plot the scatter plot
