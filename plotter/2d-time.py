import sys
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib import cm

def plot_csv(filename):
    # Read data from CSV file
    data = pd.read_csv(filename, names=['x', 'y', 'time'])

    # Extract x, y, and time columns
    x = data['x'][:100000]
    y = data['y'][:100000]
    time = data['time'][:100000]

    # Create a scatter plot with time represented as color
    scatter = plt.scatter(x, y, c=time, cmap=cm.viridis, marker='o')

    # Add colorbar to indicate the mapping of time to colors
    cbar = plt.colorbar(scatter, label='Time')

    # Add labels and title
    plt.xlabel('X-axis')
    plt.ylabel('Y-axis')
    plt.title('Scatter Plot with Time as Color')

    # Display the plot
    plt.show()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python script.py <filename.csv>")
        sys.exit(1)

    filename = sys.argv[1]
    plot_csv(filename)
