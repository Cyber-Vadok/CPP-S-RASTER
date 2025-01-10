import pandas as pd
import numpy as np
from math import pow
import argparse

def surject(x, precision):
    """Riduce la precisione di un numero decimale."""
    return int(pow(10, precision) * x)

def inverse_surject(x, precision):
    """Ripristina il numero originale dalla precisione ridotta."""
    return float(x) / pow(10, precision)

def process_dataset(file_path, giorni, punti_per_cluster, num_cluster, precisione):
    # Carica il dataset
    df = pd.read_csv(file_path, header=None)

    # Applica la riduzione della precisione alle coordinate (prime due colonne)
    df[0] = df[0].apply(lambda val: inverse_surject(surject(val, precisione), precisione))  # Coordinate x
    df[1] = df[1].apply(lambda val: inverse_surject(surject(val, precisione), precisione))  # Coordinate y

    # Rimuovi duplicati
    df = df.drop_duplicates()

    # Aggiungi il cluster ID, resettato ogni volta che cambia il tempo (terza colonna)
    cluster_ids = []
    cluster_id = 0

    for i in range(1, len(df)):
        cluster_ids.append(cluster_id)

        # Resetta il cluster ID ogni volta che il tempo cambia (usa il valore nella terza colonna, che è df.iloc[:, 2])
        if df.iloc[i, 2] != df.iloc[i-1, 2]:
            cluster_id += 1

    # Aggiungi l'ultimo cluster ID
    cluster_ids.append(cluster_id)

    # Aggiungi la colonna dei cluster ID al DataFrame senza sovrascrivere la colonna tempo
    df['cluster_id'] = cluster_ids

    return df

# Esempio di utilizzo
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Processa un dataset con clustering e precisione ridotta.")
    parser.add_argument("file_path", type=str, help="Percorso del file CSV del dataset.")
    parser.add_argument("giorni", type=int, help="Numero di giorni.")
    parser.add_argument("punti_per_cluster", type=int, help="Numero di punti per cluster.")
    parser.add_argument("num_cluster", type=int, help="Numero di cluster.")
    parser.add_argument("precisione", type=float, help="Precisione per ridurre le coordinate.")
    parser.add_argument("output_file", type=str, help="Percorso del file di output.")

    args = parser.parse_args()

    df_result = process_dataset(args.file_path, args.giorni, args.punti_per_cluster, args.num_cluster, args.precisione)
    df_result.to_csv(args.output_file, index=False, header=False)  # Senza intestazione nelle colonne
    print(f"Dataset processato e salvato in '{args.output_file}'")
