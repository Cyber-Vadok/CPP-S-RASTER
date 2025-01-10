import sys
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib import cm

def plot_csv(filename, sample_fraction=0.5):
    # Read data from CSV file
    data = pd.read_csv(filename)

    # Convert 'time' column to numeric, forcing errors to NaN
    data['time'] = pd.to_numeric(data['time'], errors='coerce')

    # Convert 'cluster_id' column to numeric, forcing errors to NaN
    data['cluster_id'] = pd.to_numeric(data['cluster_id'], errors='coerce')

    # Drop rows where 'time' or 'cluster_id' is NaN
    data = data.dropna(subset=['time', 'cluster_id'])

    # Perform sampling (default is 50% of the data)
    sampled_data = data.sample(frac=sample_fraction, random_state=0)

    # Extract x, y, time, and cluster_id columns
    x = sampled_data['x']
    y = sampled_data['y']
    time = sampled_data['time']
    cluster_id = sampled_data['cluster_id']

    # Calculate the number of clusters for each unique time value
    cluster_counts = sampled_data.groupby('time')['cluster_id'].max() + 1

    # Create a scatter plot with time represented as color
    fig, ax = plt.subplots()
    scatter = ax.scatter(x, y, c=time, cmap=cm.viridis, marker='o')

    # Add colorbar to indicate the mapping of time to colors
    cbar = plt.colorbar(scatter, ax=ax, label='Time')

    # Add labels and title
    ax.set_xlabel('X-axis')
    ax.set_ylabel('Y-axis')
    ax.set_title('Scatter Plot with Time as Color')

    # Display cluster counts as text beside the plot
    cluster_counts_text = "\n".join([f"Time {t}: {int(count)} clusters" for t, count in cluster_counts.items()])
    fig.text(0.85, 0.5, cluster_counts_text, fontsize=10, verticalalignment='center', horizontalalignment='left', transform=fig.transFigure)

    # Adjust layout to fit text
    plt.tight_layout(rect=[0, 0, 0.9, 1])

    # Display the plot
    plt.show()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python script.py <filename.csv>")
        sys.exit(1)

    filename = sys.argv[1]

    # You can change the sampling fraction here if needed, e.g., 0.01 for 1% of the data
    plot_csv(filename, sample_fraction=0.5)