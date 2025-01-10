import random
import matplotlib.pyplot as plt
import pandas as pd
import argparse

def visualizza_punti(file_csv, percentuale=50.0):
    """
    Visualizza un sottoinsieme di un dataset di punti (x, y) da un file CSV.

    Parameters:
        file_csv (str): Percorso al file CSV con due colonne per le coordinate.
        percentuale (float): Percentuale del dataset da rappresentare (0-100).
    """
    # Carica il dataset dal file CSV senza assumere intestazioni specifiche
    dataset = pd.read_csv(file_csv, header=None, names=['x', 'y'])

    if not (0 < percentuale <= 100):
        raise ValueError("La percentuale deve essere un valore tra 0 e 100.")

    # Calcola il numero di punti da selezionare
    num_punti = int(len(dataset) * (percentuale / 100))

    # Seleziona un campione casuale dal dataset
    campione = dataset.sample(n=num_punti, random_state=42)

    # Visualizza i punti
    plt.figure(figsize=(8, 6))
    plt.scatter(campione['x'], campione['y'], alpha=0.6, edgecolor='k')
    plt.title(f"Visualizzazione di {percentuale}% del dataset ({num_punti} punti)")
    plt.xlabel("x")
    plt.ylabel("y")
    plt.grid(True)
    plt.show()

# Esempio di utilizzo
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Visualizza un sottoinsieme di un dataset di punti (x, y).")
    parser.add_argument("file_csv", type=str, help="Percorso al file CSV con due colonne per le coordinate.")
    parser.add_argument("-p", "--percentuale", type=float, default=50.0, help="Percentuale del dataset da rappresentare (default: 50%).")

    args = parser.parse_args()

    # Chiama la funzione
    visualizza_punti(args.file_csv, args.percentuale)
