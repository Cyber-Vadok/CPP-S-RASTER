import pandas as pd
from sklearn.metrics import silhouette_score, davies_bouldin_score
import numpy as np
import os

# Funzione per calcolare il silhouette score per un file CSV di risultati
def calculate_silhouette(file_path):
    df = pd.read_csv(file_path)
    X = df[["x", "y"]].values  # Coordinate x, y
    labels = df["cluster_id"].values  # Cluster ID

    # Calcola il silhouette score medio
    silhouette_avg = silhouette_score(X, labels)
    return silhouette_avg

def calculate_davies(file_path):
    df = pd.read_csv(file_path)
    X = df[["x", "y"]].values  # Coordinate x, y
    labels = df["cluster_id"].values  # Cluster ID

    # Calcola il silhouette score medio
    davies = davies_bouldin_score(X, labels)
    return davies

# Funzione per eseguire il grid search su diversi parametri
def grid_search(param_ranges, base_filename="results"):
    best_score = -1
    best_params = None

    for mu in param_ranges['mu']:
        for tau in param_ranges['tau']:
            for delta in param_ranges['delta']:
                # Simula il nome del file in base ai parametri
                file_name = f"../output/{base_filename}_mu{mu}_tau{tau}_delta{delta}.csv"
                
                # Esegui l'algoritmo con questi parametri e salva il risultato in un file
                # (Nel tuo caso, dovrai eseguire il programma C++ e generare il file corrispondente)
                
                # Verifica se il file esiste
                if os.path.exists(file_name):
                    score = calculate_silhouette(file_name)
                    print(f"Silhouette score for mu={mu}, tau={tau}, delta={delta}: {score}")
                    
                    if score > best_score:
                        best_score = score
                        best_params = (mu, tau, delta)

    return best_params, best_score

# Definizione degli intervalli per i parametri
param_ranges = {
    'mu': range(1, 11),
    'tau': range(1, 11),
    'delta': range(1, 9)
}

# Esegui il grid search
best_params, best_score = grid_search(param_ranges)
print(f"Best parameters: mu={best_params[0]}, tau={best_params[1]}, delta={best_params[2]}")
print(f"Best silhouette score: {best_score}")
