import subprocess
import pandas as pd
import argparse
from sklearn.metrics import silhouette_score, silhouette_samples
import numpy as np
from skopt import gp_minimize
from skopt.space import Real, Integer
from skopt.utils import use_named_args
import time
import csv

# Define a penalty value
PENALTY = 1
BIG_PENALTY = 3

def evaluate_sraster(params, expected_clusters, input_file, output_file):
    mu, xi, tau, delta = params
    # Costruzione del comando per eseguire l'algoritmo con i parametri
    command = [
        './build/bin/sraster',
        '-m', str(mu),
        '-p', str(xi),
        '-t', str(tau),
        '-d', str(delta),
        '-f', input_file,
        '-o', output_file
    ]
    
    # Print the command
    print("Executing command:", " ".join(command))
    
    # Esegui l'algoritmo SRaster e cattura il risultato
    start_time = time.time()
    result = subprocess.run(command, capture_output=True, text=True)
    execution_time = time.time() - start_time

    print(f"Execution time: {execution_time:.2f} seconds")
    
    # Check if the output file is empty
    try:
        df = pd.read_csv(output_file)
        if df.empty:
            print("Output file is empty.")
            return BIG_PENALTY, execution_time, 0, 0  # Penalizza se il file è vuoto
    except Exception as e:
        print(f"Error reading output file: {e}")
        return BIG_PENALTY, execution_time, 0, 0  # Penalizza se c'è un errore nella lettura del file
    
    # Calcola il numero totale di cluster considerando il reset di cluster_id per ogni time
    unique_clusters = df.groupby('time')['cluster_id'].nunique().sum()
    print(f"Number of clusters found: {unique_clusters}")

    # Check if the number of samples is valid relative to the number of clusters for each time value
    unique_times = df["time"].unique()
    for time_value in unique_times:
        time_df = df[df["time"] == time_value]
        num_samples = len(time_df)
        num_clusters = time_df["cluster_id"].nunique()
        if num_clusters < 2 or num_clusters >= num_samples:
            print(f"Number of clusters ({num_clusters}) is not valid for the number of samples ({num_samples}) for time {time_value}.")
            return PENALTY, execution_time, len(df), unique_clusters  # Penalizza se il numero di cluster non è compreso tra 2 e n-1 per qualsiasi time value

    # per prima cosa controllare se il numero di cluster e' giusto
    if unique_clusters < 2:
        print(f"Not enough clusters to calculate silhouette score. Found: {unique_clusters}")
        return PENALTY, execution_time, len(df), unique_clusters  # Penalizza se ci sono meno di 2 cluster
    if unique_clusters != expected_clusters:
        print(f"Expected clusters: {expected_clusters}, but found: {unique_clusters}")
        return PENALTY, execution_time, len(df), unique_clusters  # Penalizza se il numero di cluster non è come previsto
    else:
        # Assicurati che le colonne siano nel formato corretto
        df['time'] = df['time'].astype(int)  # Assicurati che 'time' sia numerico

        # Usa solo le colonne delle coordinate e del cluster ID
        X = df[["x", "y"]].values  # Array con coordinate x e y
        labels = df["cluster_id"].values  # Array con l'ID del cluster
        time_values = df["time"].values  # Array con i valori di time

        # Crea una lista per memorizzare i silhouette score medi per ciascun time
        silhouette_avg_per_time = []

        # Crea una lista per visualizzare il silhouette score per ogni punto
        silhouette_values = silhouette_samples(X, labels)

        # Calcola il silhouette score per ogni valore di time
        for time_value in unique_times:
            # Seleziona i dati corrispondenti a questo time
            time_mask = time_values == time_value
            X_time = X[time_mask]
            labels_time = labels[time_mask]

            # Controllo sulla validità dei campioni e cluster
            if len(labels_time) < 3 or len(np.unique(labels_time)) < 2:
                print(f"Not enough samples ({len(labels_time)}) or clusters ({len(np.unique(labels_time))}) for silhouette_score at time {time_value}.")
                silhouette_avg_per_time.append(PENALTY)  # Penalizza per quel time
                continue

            # Calcola il silhouette score per questo time
            silhouette_avg = silhouette_score(X_time, labels_time)
            silhouette_avg_per_time.append(silhouette_avg)
            print(f"Silhouette Score per il time {time_value}: {silhouette_avg}")

        # Calcola il silhouette score medio per l'intero dataset
        silhouette_avg_overall = np.mean(silhouette_avg_per_time)
        print(f"Silhouette Score medio complessivo: {silhouette_avg_overall}")
        return -silhouette_avg_overall, execution_time, len(df), unique_clusters  # Minimize negative silhouette score

# Parse command-line arguments
parser = argparse.ArgumentParser(description='Optimize SRaster parameters.')
parser.add_argument('expected_clusters', type=int, help='The expected number of clusters.')
parser.add_argument('input_file', type=str, help='Path to the input CSV file.')
parser.add_argument('output_file', type=str, help='Path to the output CSV file.')
parser.add_argument('results_path', type=str, help='Path to save the results CSV file.')
parser.add_argument('--mu-lb', type=int, default=1, help='Lower bound for mu.')
parser.add_argument('--mu-ub', type=int, default=100, help='Upper bound for mu.')
parser.add_argument('--xi-lb', type=float, default=1.0, help='Lower bound for xi.')
parser.add_argument('--xi-ub', type=float, default=5.0, help='Upper bound for xi.')
parser.add_argument('--tau-lb', type=int, default=1, help='Lower bound for tau.')
parser.add_argument('--tau-ub', type=int, default=100, help='Upper bound for tau.')
parser.add_argument('--delta-lb', type=int, default=1, help='Lower bound for delta.')
parser.add_argument('--delta-ub', type=int, default=5, help='Upper bound for delta.')
args = parser.parse_args()

expected_clusters = args.expected_clusters
input_file = args.input_file
output_file = args.output_file
results_path = args.results_path

# Define the search space for the optimization
space = [
    Integer(args.mu_lb, args.mu_ub, name='mu'),
    Real(args.xi_lb, args.xi_ub, name='xi'),
    Integer(args.tau_lb, args.tau_ub, name='tau'),
    Integer(args.delta_lb, args.delta_ub, name='delta')
]

# List to store optimization results
results = []

# Funzione di ottimizzazione bayesiana
@use_named_args(space)
def objective_function(mu, xi, tau, delta):
    score, execution_time, num_points, num_clusters = evaluate_sraster([mu, xi, tau, delta], expected_clusters, input_file, output_file)
    results.append({"mu": mu, "xi": xi, "tau": tau, "delta": delta, "score": -score, "execution_time": execution_time, "num_points": num_points, "num_clusters": num_clusters})
    return score

# Esegui l'ottimizzazione bayesiana
res = gp_minimize(objective_function, space, n_calls=100, random_state=7)

# Ordina i risultati per score
sorted_results = sorted(results, key=lambda x: x['score'], reverse=True)

# Salva i risultati in un file CSV
with open(results_path, mode='w', newline='') as file:
    writer = csv.DictWriter(file, fieldnames=["mu", "xi", "tau", "delta", "score", "execution_time", "num_points", "num_clusters"])
    writer.writeheader()
    writer.writerows(sorted_results)

print(f"Results saved to {results_path}")

# Print the best result
print(f"Best parameters: {res.x}")
print(f"Best score: {-res.fun}")