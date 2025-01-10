import pandas as pd
from sklearn.metrics import silhouette_score, silhouette_samples
import matplotlib.pyplot as plt
import numpy as np
import argparse

# Configura il parser per gli argomenti da linea di comando
parser = argparse.ArgumentParser(description="Calcola il silhouette score da un dataset.")
parser.add_argument("dataset", type=str, help="Percorso del file CSV contenente il dataset")

# Leggi gli argomenti
args = parser.parse_args()

# Leggi il dataset dal file specificato
df = pd.read_csv(args.dataset)

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
unique_times = np.unique(time_values)  # Trova i tempi unici
for time in unique_times:
    # Seleziona i dati corrispondenti a questo time
    # https://www.educative.io/answers/what-is-boolean-masking-on-numpy-arrays-in-python
    time_mask = time_values == time
    X_time = X[time_mask]
    labels_time = labels[time_mask]

    # Calcola il silhouette score per questo time
    silhouette_avg = silhouette_score(X_time, labels_time)
    silhouette_avg_per_time.append(silhouette_avg)
    print(f"Silhouette Score per il time {time}: {silhouette_avg}")

# Calcola il silhouette score medio per l'intero dataset
silhouette_avg_overall = np.mean(silhouette_avg_per_time)
print(f"Silhouette Score medio complessivo: {silhouette_avg_overall}")

# # Visualizza i risultati in un grafico a barre
# plt.bar(unique_times, silhouette_avg_per_time, color='blue', alpha=0.7)
# plt.axhline(y=silhouette_avg_overall, color='red', linestyle="--", label="Silhouette Score Medio")
# plt.text(
#     x=max(unique_times) + 0.1,  # Posizione x a destra del grafico
#     y=silhouette_avg_overall,  # Posizione y sulla linea
#     s=f"{silhouette_avg_overall:.2f}",  # Valore del silhouette score medio
#     color="red",
#     fontsize=12,
#     verticalalignment="center"
# )
# plt.xlabel("time")
# plt.ylabel("Silhouette Score")
# plt.title("Silhouette Score per ciascun intervallo di time")
# plt.legend()
# plt.show()