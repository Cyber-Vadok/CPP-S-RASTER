from sklearn.metrics import adjusted_rand_score
from sklearn.cluster import YourClusteringAlgorithm  # Replace with your clustering algorithm
import numpy as np

# Generate some sample data (replace this with your own dataset)
data = np.array([[1, 2], [5, 8], [1.5, 1.8], [8, 8], [1, 0.6], [9, 11]])
true_labels = np.array([0, 1, 0, 1, 0, 1])  # True cluster assignments

# Instantiate your clustering algorithm (replace YourClusteringAlgorithm with your algorithm)
clustering_algorithm = YourClusteringAlgorithm()

# Fit your clustering algorithm and get predicted labels
predicted_labels = clustering_algorithm.fit_predict(data)

# Calculate Adjusted Rand Index
ari = adjusted_rand_score(true_labels, predicted_labels)

print("Adjusted Rand Index (ARI):", ari)
