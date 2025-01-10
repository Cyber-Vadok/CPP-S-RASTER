import subprocess
import pandas as pd
import argparse
from sklearn.metrics import silhouette_score
from skopt import gp_minimize
from skopt.space import Real, Integer

# Define a function to round to one decimal place
def round_to_one_decimal(x):
    return round(x, 1)

from skopt.utils import use_named_args
import time

# Define the search space for the optimization
space = [
    Integer(2, 10, name='mu'),
    Real(2, 5.0, name='xi'),
    Integer(2, 10, name='tau'),
    Integer(1, 3, name='delta')
]

def evaluate_sraster(params, expected_clusters):
    mu, xi, tau, delta = params
    # Costruzione del comando per eseguire l'algoritmo con i parametri
    command = [
        './build/bin/sraster',
        '-m', str(mu),
        '-p', str(round_to_one_decimal(xi)),
        '-t', str(tau),
        '-d', str(delta),
        '-f', 'data_generator/data.csv',
        '-o', 'output.csv'
    ]
    
    # Print the command
    print(f"Executing command: {' '.join(command)}")
    
    # Esegui l'algoritmo SRaster e cattura il risultato
    start_time = time.time()
    result = subprocess.run(command)
    execution_time = time.time() - start_time

    print(f"Execution time: {execution_time:.2f} seconds")
    
    # Check if the output file is empty
    try:
        df = pd.read_csv('output.csv')
        if df.empty:
            print("Output file is empty.")
            return float('inf')  # Penalize if the output file is empty
    except Exception as e:
        print(f"Error reading output file: {e}")
        return float('inf')  # Penalize if there is an error reading the file
    
    # Leggi l'ultima riga di output.csv
    last_row = df.iloc[-1]
    last_id = last_row['cluster_id']

    # Calcola il numero di cluster
    num_clusters = last_id + 1
    print(f"Number of clusters found: {num_clusters}")

    # Verifica se il numero di cluster è corretto
    if num_clusters != expected_clusters:
        print(f"Expected clusters: {expected_clusters}, but found: {num_clusters}")
        return float('inf')  # Penalize if the number of clusters is not as expected
    else:
        # Calcola il silhouette score
        X = df[['x', 'y']].values
        labels = df['cluster_id'].values
        try:
            silhouette_avg = silhouette_score(X, labels)
            print(f"Silhouette Score: {silhouette_avg}")
            return -silhouette_avg  # Minimize negative silhouette score
        except Exception as e:
            print(f"Error calculating silhouette score: {e}")
            return float('inf')

# Funzione di ottimizzazione bayesiana
@use_named_args(space)
def objective_function(mu, xi, tau, delta):
    print(f"Evaluating parameters: mu={mu}, xi={round_to_one_decimal(xi)}, tau={tau}, delta={delta}")
    return evaluate_sraster([mu, xi, tau, delta], expected_clusters)

# Parse command-line arguments
parser = argparse.ArgumentParser(description='Optimize SRaster parameters.')
parser.add_argument('expected_clusters', type=int, help='The expected number of clusters.')
args = parser.parse_args()
expected_clusters = args.expected_clusters

# Inizia l'ottimizzazione bayesiana
print(f"Starting Bayesian optimization with expected clusters: {expected_clusters}")
res = gp_minimize(objective_function, space, n_calls=50, random_state=42)

# Risultato finale
print("\nOptimization completed!")
print(f"Best parameters found: mu={res.x[0]}, xi={res.x[1]}, tau={res.x[2]}, delta={res.x[3]}")
print(f"Best silhouette score (negative): {-res.fun}")
