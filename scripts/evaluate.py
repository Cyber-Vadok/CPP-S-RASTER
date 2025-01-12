import numpy as np
import pandas as pd
from sklearn.metrics import pairwise_distances, silhouette_score, silhouette_samples
from sklearn.metrics.pairwise import euclidean_distances
from sklearn.cluster import KMeans
import argparse


def beta_cv(X, labels):
    """
    Compute the BetaCV measure, which is the ratio of the mean intra-cluster distance 
    to the mean inter-cluster distance.
    
    Parameters:
    X : array-like, shape (n_samples, n_features)
        Data points to evaluate.
    labels : array-like, shape (n_samples,)
        Cluster labels for each point in X.
        
    Returns:
    float
        The BetaCV measure value.
    """
    unique_labels = np.unique(labels)
    W_in_sum = 0  # Sum of intra-cluster distances
    W_out_sum = 0  # Sum of inter-cluster distances
    N_in_sum = 0  # Sum of points in each cluster
    N_out_sum = 0  # Sum of points in other clusters
    
    for label in unique_labels:
        cluster_points = X[labels == label]
        
        # Intra-cluster distance: Pairwise distances within the cluster
        dist_in = pairwise_distances(cluster_points)
        W_in = np.sum(dist_in) / 2  # sum of pairwise distances, divide by 2 to avoid double counting
        N_in = len(cluster_points)
        
        W_in_sum += W_in
        N_in_sum += N_in
        
        for other_label in unique_labels:
            if label != other_label:
                other_cluster_points = X[labels == other_label]
                dist_out = euclidean_distances(cluster_points, other_cluster_points)
                W_out = np.sum(dist_out)
                N_out = len(other_cluster_points)
                
                W_out_sum += W_out
                N_out_sum += N_out
    
    beta_cv_value = (N_out_sum / N_in_sum) * (W_in_sum / W_out_sum)
    return beta_cv_value


def calculate_scores(input_file, unique_times):
    """
    Calculate both the Silhouette Score and BetaCV for each unique time value.
    
    Parameters:
    input_file : str
        Path to the CSV input file containing the dataset.
    unique_times : array-like
        List of unique 'time' values to evaluate.
        
    Returns:
    tuple
        A tuple containing two lists: silhouette_avg_per_time and beta_cv_per_time
    """
    df = pd.read_csv(input_file)

    # Assicurati che le colonne siano nel formato corretto
    df['time'] = df['time'].astype(int)  # Assicurati che 'time' sia numerico

    # Usa solo le colonne delle coordinate e del cluster ID
    X = df[["x", "y"]].values  # Array con coordinate x e y
    labels = df["cluster_id"].values  # Array con l'ID del cluster
    time_values = df["time"].values  # Array con i valori di time

    # Crea le liste per memorizzare i risultati
    silhouette_avg_per_time = []
    beta_cv_per_time = []

    for time_value in unique_times:
        # Seleziona i dati corrispondenti a questo time
        time_mask = time_values == time_value
        X_time = X[time_mask]
        labels_time = labels[time_mask]

        # Calcolo del silhouette score
        if len(labels_time) < 3 or len(np.unique(labels_time)) < 2:
            print(f"Not enough samples or clusters for silhouette_score at time {time_value}.")
            silhouette_avg_per_time.append(None)  # Penalizza per quel time
        else:
            silhouette_avg = silhouette_score(X_time, labels_time)
            silhouette_avg_per_time.append(silhouette_avg)
            print(f"Silhouette Score per il time {time_value}: {silhouette_avg}")

        beta_cv_value = beta_cv(X_time, labels_time)
        beta_cv_per_time.append(beta_cv_value)
        print(f"BetaCV per il time {time_value}: {beta_cv_value}")

    return silhouette_avg_per_time, beta_cv_per_time


if __name__ == "__main__":
    # Gestione degli argomenti da riga di comando
    parser = argparse.ArgumentParser(description="Calculate Silhouette Score and BetaCV for a clustering dataset.")
    parser.add_argument('input_file', type=str, help="Path to the input CSV file containing the dataset.")
    parser.add_argument('output_file', type=str, help="Path to the output CSV file for saving results.")
    
    args = parser.parse_args()
    input_file = args.input_file
    output_file = args.output_file

    # Carica i valori unici di time dal file di input
    unique_times = np.unique(pd.read_csv(input_file)["time"].values)

    # Calcola il silhouette score e il BetaCV per ogni valore di time
    silhouette_avg_per_time, beta_cv_per_time = calculate_scores(input_file, unique_times)

    # Scrivi i risultati nel file di output
    results_df = pd.DataFrame({
        'Time': unique_times,
        'Silhouette_Score': silhouette_avg_per_time,
        'BetaCV': beta_cv_per_time
    })

    results_df.to_csv(output_file, index=False)

    # Stampa i risultati
    print("\nSilhouette Scores per Time:")
    print(silhouette_avg_per_time)
    print("\nBetaCV per Time:")
    print(beta_cv_per_time)

    print(f"\nResults saved to {output_file}")
