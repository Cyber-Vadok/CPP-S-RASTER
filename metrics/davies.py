import pandas as pd
from sklearn.metrics import silhouette_score, davies_bouldin_score
import numpy as np
import os
import sys

def calculate_davies(file_path):
    df = pd.read_csv(file_path)
    X = df[["x", "y"]].values  # Coordinate x, y
    labels = df["cluster_id"].values  # Cluster ID

    # Calcola il silhouette score medio
    davies = davies_bouldin_score(X, labels)
    return davies

if __name__ == "__main__":
    if len(sys.argv) == 2:
        file_path = sys.argv[1]
        score = calculate_davies(file_path)
        print(score)
    else:
        print("Usage: python davies.py [input_file]")
        sys.exit(1)
