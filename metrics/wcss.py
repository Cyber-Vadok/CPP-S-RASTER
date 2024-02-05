from sklearn.cluster import KMeans
import numpy as np

# Sample data (replace this with your own dataset)
data = np.array([[1, 2], [5, 8], [1.5, 1.8], [8, 8], [1, 0.6], [9, 11]])

# Specify the number of clusters (k)
k = 2

# Fit KMeans model
kmeans = KMeans(n_clusters=k)
kmeans.fit(data)

# Get cluster centers and labels
centroids = kmeans.cluster_centers_
labels = kmeans.labels_

# Calculate WCSS
wcss = 0
for i in range(k):
    cluster_points = data[labels == i]
    cluster_center = centroids[i]
    wcss += np.sum((cluster_points - cluster_center) ** 2)

print("Within-Cluster Sum of Squares (WCSS):", wcss)
