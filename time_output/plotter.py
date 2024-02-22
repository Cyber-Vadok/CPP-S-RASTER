import pandas as pd
import matplotlib.pyplot as plt
import os

# Function to process a single file
def process_file(file_path):
    # Read the CSV file into a DataFrame
    df = pd.read_csv(file_path)

    # Calculate mean and variance for each column
    mean_elapsed_time = df['Elapsed Time (ms)'].mean()
    mean_ops_per_sec = df['Operations per second'].mean()

    var_elapsed_time = df['Elapsed Time (ms)'].var()
    var_ops_per_sec = df['Operations per second'].var()

    # Plot sequence of elapsed time
    plt.figure(figsize=(10, 5))
    plt.plot(df['Elapsed Time (ms)'], label='Elapsed Time')
    plt.axhline(mean_elapsed_time, color='red', linestyle='--', label='Mean')
    plt.fill_between(range(len(df)), mean_elapsed_time-var_elapsed_time, mean_elapsed_time+var_elapsed_time, color='red', alpha=0.2, label='Variance')
    plt.xlabel('Data Points')
    plt.ylabel('Elapsed Time (ms)')
    plt.title('Elapsed Time (ms) - {}'.format(os.path.basename(file_path)))
    plt.legend()
    plt.savefig('plots/{}_elapsed_time.png'.format(os.path.basename(file_path).split('.')[0]))

    # Plot sequence of operations per second
    plt.figure(figsize=(10, 5))
    plt.plot(df['Operations per second'], label='Operations per second')
    plt.axhline(mean_ops_per_sec, color='blue', linestyle='--', label='Mean')
    plt.fill_between(range(len(df)), mean_ops_per_sec-var_ops_per_sec, mean_ops_per_sec+var_ops_per_sec, color='blue', alpha=0.2, label='Variance')
    plt.xlabel('Data Points')
    plt.ylabel('Operations per second')
    plt.title('Operations per second - {}'.format(os.path.basename(file_path)))
    plt.legend()
    plt.savefig('plots/{}_ops_per_sec.png'.format(os.path.basename(file_path).split('.')[0]))

    # Return mean and variance
    return mean_elapsed_time, var_elapsed_time, mean_ops_per_sec, var_ops_per_sec

# Function to process all files with the same timestamp
def process_files_with_timestamp(timestamp):
    files_with_timestamp = [file for file in os.listdir('.') if timestamp in file and file.endswith('.csv')]
    mean_var_data = []

    for file in files_with_timestamp:
        mean_elapsed_time, var_elapsed_time, mean_ops_per_sec, var_ops_per_sec = process_file(file)
        mean_var_data.append((file, mean_elapsed_time, var_elapsed_time, mean_ops_per_sec, var_ops_per_sec))

    return mean_var_data

# Main function
def main():
    # Create directory to store plots if it doesn't exist
    if not os.path.exists('plots'):
        os.makedirs('plots')

    # Get all unique timestamps from file names
    file_names = [file for file in os.listdir('.') if file.endswith('.csv')]
    timestamps = set(file_name.split('_')[-2] for file_name in file_names)

    # Process files for each timestamp
    all_mean_var_data = []
    for timestamp in timestamps:
        mean_var_data = process_files_with_timestamp(timestamp)
        all_mean_var_data.extend(mean_var_data)

    # Plot mean and variance for all files
    plt.figure(figsize=(10, 5))
    plt.errorbar(range(len(all_mean_var_data)), [mean for _, mean, _, _, _ in all_mean_var_data], yerr=[var for _, _, var, _, _ in all_mean_var_data], fmt='o', color='green', capsize=5)
    plt.xticks(range(len(all_mean_var_data)), [file_name for file_name, _, _, _, _ in all_mean_var_data], rotation=45)
    plt.xlabel('File')
    plt.ylabel('Elapsed Time (ms) Mean and Variance')
    plt.title('Mean and Variance of Elapsed Time (ms) for all files')
    plt.savefig('plots/mean_var_elapsed_time_all_files.png')

    plt.figure(figsize=(10, 5))
    plt.errorbar(range(len(all_mean_var_data)), [mean for _, _, _, mean, _ in all_mean_var_data], yerr=[var for _, _, _, _, var in all_mean_var_data], fmt='o', color='orange', capsize=5)
    plt.xticks(range(len(all_mean_var_data)), [file_name for file_name, _, _, _, _ in all_mean_var_data], rotation=45)
    plt.xlabel('File')
    plt.ylabel('Operations per second Mean and Variance')
    plt.title('Mean and Variance of Operations per second for all files')
    plt.savefig('plots/mean_var_ops_per_sec_all_files.png')

if __name__ == "__main__":
    main()
