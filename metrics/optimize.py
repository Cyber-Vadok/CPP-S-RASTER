import subprocess
import pandas as pd
import argparse
from sklearn.metrics import silhouette_score
import numpy as np
from skopt import gp_minimize
from skopt.space import Real, Integer
from skopt.utils import use_named_args
import time
import csv

def round_to_decimal(value, decimals=1):
    """
    Arrotonda un valore al numero specificato di cifre decimali.
    """
    return round(value, decimals)

# Define the search space for the optimization
space = [
    Integer(2, 10, name='mu'),
    Real(2, 5, name='xi'),  # Verrà arrotondato a una sola cifra decimale
    Integer(2, 10, name='tau'),
    Integer(1, 5, name='delta')
]

# List to store optimization results
results = []

def evaluate_sraster(params, expected_clusters):
    mu, xi, tau, delta = params
    xi = round_to_decimal(xi, 1)  # Arrotonda xi a una cifra decimale
    # Costruzione del comando per eseguire l'algoritmo con i parametri
    command = [
        './build/bin/sraster',
        '-m', str(mu),
        '-p', str(xi),
        '-t', str(tau),
        '-d', str(delta),
        '-f', 'data_generator/data.csv',
        '-o', 'output.csv'
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
        df = pd.read_csv('output.csv')
        if df.empty:
            print("Output file is empty.")
            return float('inf')  # Penalizza se il file è vuoto
    except Exception as e:
        print(f"Error reading output file: {e}")
        return float('inf')  # Penalizza se c'è un errore nella lettura del file
    
    # Leggi l'ultima riga di output.csv
    last_row = df.iloc[-1]
    last_id = last_row['cluster_id']

    # Calcola il numero di cluster
    num_clusters = last_id + 1
    print(f"Number of clusters found: {num_clusters}")

    if num_clusters != expected_clusters:
        print(f"Expected clusters: {expected_clusters}, but found: {num_clusters}")
        return float('inf')  # Penalizza se il numero di cluster è errato
    else:
        # Assicurati che le colonne siano nel formato corretto
        df['time'] = df['time'].astype(int)

        X = df[["x", "y"]].values  # Array con coordinate x e y
        labels = df["cluster_id"].values  # Array con l'ID del cluster
        time_values = df["time"].values  # Array con i valori di time

        silhouette_avg_per_time = []

        # Calcola il silhouette score per ogni valore unico di time
        unique_times = np.unique(time_values)
        for time_value in unique_times:
            time_mask = time_values == time_value
            X_time = X[time_mask]
            labels_time = labels[time_mask]

            silhouette_avg = silhouette_score(X_time, labels_time)
            silhouette_avg_per_time.append(silhouette_avg)
            print(f"Silhouette Score per il time {time_value}: {silhouette_avg}")

        silhouette_avg_overall = np.mean(silhouette_avg_per_time)
        print(f"Silhouette Score medio complessivo: {silhouette_avg_overall}")
        return -silhouette_avg_overall  # Minimizza il valore negativo del silhouette score

# Funzione di ottimizzazione bayesiana
@use_named_args(space)
def objective_function(mu, xi, tau, delta):
    xi = round_to_decimal(xi, 1)  # Arrotonda xi a una cifra decimale
    score = evaluate_sraster([mu, xi, tau, delta], expected_clusters)
    results.append({"mu": mu, "xi": xi, "tau": tau, "delta": delta, "score": -score})
    return score

# Parse command-line arguments
parser = argparse.ArgumentParser(description='Optimize SRaster parameters.')
parser.add_argument('expected_clusters', type=int, help='The expected number of clusters.')
args = parser.parse_args()
expected_clusters = args.expected_clusters

# Esegui l'ottimizzazione bayesiana
res = gp_minimize(objective_function, space, n_calls=50, random_state=42)

# Ordina i risultati per score
sorted_results = sorted(results, key=lambda x: x['score'], reverse=True)

# Salva i risultati in un file CSV
output_file = "results.csv"
with open(output_file, mode='w', newline='') as file:
    writer = csv.DictWriter(file, fieldnames=["mu", "xi", "tau", "delta", "score"])
    writer.writeheader()
    writer.writerows(sorted_results)

print(f"Results saved to {output_file}")

# Print the best result
print(f"Best parameters: {res.x}")
print(f"Best score: {-res.fun}")
