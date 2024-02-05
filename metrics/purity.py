# • "purity" Average purity of clusters. The purity of each cluster is the proportion of the points
# of the majority true group assigned to it (see Cao et al. (2006)).

from sklearn.metrics import confusion_matrix
import numpy as np

def calculate_purity(true_labels, predicted_labels):
    """
    Calculate the purity of clusters.

    Parameters:
    - true_labels: True labels or ground truth.
    - predicted_labels: Predicted cluster labels.

    Returns:
    - purity: Purity of clusters.
    """
    # Create a confusion matrix
    confusion_mat = confusion_matrix(true_labels, predicted_labels)

    # Calculate purity
    purity = np.sum(np.max(confusion_mat, axis=0)) / np.sum(confusion_mat)

    return purity

# Example usage:
# Replace true_labels and predicted_labels with your actual data
true_labels = np.array([0, 1, 1, 0, 2, 2, 2])
predicted_labels = np.array([0, 1, 1, 0, 2, 0, 2])

purity = calculate_purity(true_labels, predicted_labels)
print("Purity of clusters:", purity)
