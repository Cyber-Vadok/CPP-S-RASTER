import pandas as pd
import matplotlib
# matplotlib.use('GTK3Agg')
import matplotlib.pyplot as plt
import os
import argparse

# Function to process a single file
def process_file(file_path):
    # Read the CSV file into a DataFrame
    df = pd.read_csv(file_path)

    # Calculate mean and standard deviation for each column
    mean_elapsed_time = df['Elapsed Time (ms)'].mean()
    mean_ops_per_sec = df['Operations per second'].mean()

    std_elapsed_time = df['Elapsed Time (ms)'].std()  # Standard deviation
    std_ops_per_sec = df['Operations per second'].std()  # Standard deviation

    # Plot sequence of elapsed time
    plt.figure(figsize=(10, 5))
    plt.plot(df['Elapsed Time (ms)'], label='Elapsed Time')

    # Add mean and standard deviation to the label
    plt.axhline(mean_elapsed_time, color='red', linestyle='--', label=f'Mean: {mean_elapsed_time:.2e}')
    
    # Add standard deviation to the label
    plt.fill_between(
        range(len(df)), 
        mean_elapsed_time - std_elapsed_time, 
        mean_elapsed_time + std_elapsed_time, 
        color='red', 
        alpha=0.2, 
        label=f'Standard Deviation: ±{std_elapsed_time:.2e}'
    )
    
    plt.xlabel('Dataset id #')
    plt.ylabel('Elapsed Time (ms)')
    plt.title(f'Elapsed Time (ms) - {os.path.basename(file_path)}')
    plt.legend()
    
    # Automatically adjust the axis based on data range
    plt.tight_layout()  # Adjust layout for better visualization
    plt.savefig(f'plots/{os.path.basename(file_path).split(".")[0]}_elapsed_time.png')
    # plt.show()  # Display the plot
    
    # Plot sequence of operations per second
    plt.figure(figsize=(10, 5))
    plt.plot(df['Operations per second'], label='Operations per second')

    # Plot mean in scientific notation
    plt.axhline(mean_ops_per_sec, color='blue', linestyle='--', label=f'Mean: {mean_ops_per_sec:.2e}')

    # Plot standard deviation region in scientific notation
    plt.fill_between(
        range(len(df)), 
        mean_ops_per_sec - std_ops_per_sec, 
        mean_ops_per_sec + std_ops_per_sec, 
        color='blue', 
        alpha=0.2, 
        label=f'Standard Deviation: ±{std_ops_per_sec:.2e}'
    )

    # Set axis limits to extend only to the min and max of the data
    plt.ylim(df['Operations per second'].min() * 0.98, df['Operations per second'].max() * 1.02)

    # Use scientific notation for the y-axis
    plt.ticklabel_format(axis='y', style='sci', scilimits=(0,0))

    # Labels and title
    plt.xlabel('Dataset id #')
    plt.ylabel('Operations per second')
    plt.title(f'Operations per second - {os.path.basename(file_path)}')
    plt.legend()
    # Automatically adjust the axis based on data range
    plt.tight_layout()  # Adjust layout for better visualization
    plt.savefig(f'plots/{os.path.basename(file_path).split(".")[0]}_ops_time.png')
    # plt.show()  # Display the plot

    # Return mean and standard deviation
    return mean_elapsed_time, std_elapsed_time, mean_ops_per_sec, std_ops_per_sec

# Main function
def main():
    # Set up argument parser
    parser = argparse.ArgumentParser(description='Process a CSV file to generate plots for elapsed time and operations per second.')
    parser.add_argument('file', help='Name of the CSV file to process')

    # Parse arguments
    args = parser.parse_args()

    # Create directory to store plots if it doesn't exist
    if not os.path.exists('plots'):
        os.makedirs('plots')

    # Process the given file
    mean_elapsed_time, std_elapsed_time, mean_ops_per_sec, std_ops_per_sec = process_file(args.file)

    # Print the results (optional)
    # print(f"File processed: {args.file}")
    # print(f"Mean Elapsed Time (ms): {mean_elapsed_time}")
    # print(f"Standard Deviation of Elapsed Time (ms): {std_elapsed_time}")
    # print(f"Mean Operations per second: {mean_ops_per_sec}")
    # print(f"Standard Deviation of Operations per second: {std_ops_per_sec}")

    # Append results to the log file
    with open('time_output/log.txt', 'a') as log_file:
        log_file.write(f"File processed: {args.file}\n")
        log_file.write(f"Mean Elapsed Time (ms): {mean_elapsed_time}\n")
        log_file.write(f"Standard Deviation of Elapsed Time (ms): {std_elapsed_time}\n")
        log_file.write(f"Mean Operations per second: {mean_ops_per_sec}\n")
        log_file.write(f"Standard Deviation of Operations per second: {std_ops_per_sec}\n")
        log_file.write("\n")  # Blank line for separation

if __name__ == "__main__":
    main()
