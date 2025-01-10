import pandas as pd

# Dati
data = {
    'Elapsed Time (ms)': [46, 46, 46, 45, 47, 47, 46, 46, 46, 47],
    'Operations per second': [1.09785e+06, 1.09785e+06, 1.09785e+06, 1.12224e+06, 1.07449e+06,
                              1.07449e+06, 1.09785e+06, 1.09785e+06, 1.09785e+06, 1.07449e+06]
}

# Creazione del DataFrame
df = pd.DataFrame(data)

# Calcolare la varianza sulla seconda colonna (Operations per second)
variance = df['Operations per second'].var()

# Stampare il risultato
print(f"Varianza delle operazioni al secondo: {variance}")
