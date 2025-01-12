
# SRaster - Guida all'uso

## Indice
1. [Installazione e compilazione](#installazione-e-compilazione)
2. [Esecuzione dell'algoritmo SRaster](#esecuzione-dellalgoritmo-sraster)
3. [Generazione del Dataset](#generazione-del-dataset)
4. [Benchmark](#benchmark)
5. [Valutazione delle metriche esterne](#valutazione-delle-metriche-esterne)

## 1. Installazione e compilazione

Per iniziare, è necessario installare le dipendenze e compilare il programma. Seguire i passaggi descritti di seguito.

### 1.1 Installazione delle dipendenze

I benchmark e l'algoritmo SRaster richiedono alcune dipendenze Python. Per installarle, eseguire il seguente comando:

```
pip install -r requirements.txt
```

### 1.2 Compilazione dell'algoritmo SRaster

L'algoritmo SRaster è implementato come programma eseguibile. Dopo aver scaricato il repository, è necessario compilare il programma con il comando `make`:

```
make
```

Una volta completata la compilazione, il programma eseguibile SRaster sarà disponibile nella directory `./bin/build/`.

## 2. Esecuzione dell'algoritmo SRaster

L'algoritmo SRaster viene eseguito utilizzando il comando seguente. È necessario specificare il file di input con l'opzione `-f`. Gli altri parametri sono opzionali e permettono di configurare ulteriormente l'esecuzione.

### 2.1 Sintassi del comando

```
./bin/build/sraster -f input-filepath [-c <time-filepath>] [-o <output-filepath>] [-m <mu>] [-p <xi>] [-t <tau>] [-d <delta>] [-w <window size>]
```

### 2.2 Parametri disponibili

- `-f filepath`: Specifica il percorso del file di input (obbligatorio).
- `-c <time-filepath>`: (Opzionale) Stampa il tempo di esecuzione e salva i dati sui tempi nel file specificato.
- `-o filepath`: (Opzionale) Specifica il percorso del file di output.
- `-m <mu>`: (Opzionale) Imposta la dimensione minima di un cluster (predefinito: 4, massimo: UINT_MAX).
- `-p <xi>`: (Opzionale) Imposta la precisione per l'operazione di proiezione (predefinito: 3.5, massimo: MAX_PRECISION).
- `-t <tau>`: (Opzionale) Imposta il numero minimo di punti per considerare una tile come significativa (predefinito: 5, massimo: UINT_MAX).
- `-d <delta>`: (Opzionale) Imposta la metrica di distanza per la definizione dei cluster (predefinito: 1, massimo: UINT_MAX).
- `-w <window size>`: (Opzionale) Specifica la dimensione della finestra (predefinito: 10).

### 2.3 Esempio di utilizzo

Esegui il comando seguente per utilizzare l'algoritmo SRaster con parametri di default:

```
./bin/build/sraster -f ./data/input.csv -o ./results/output.csv -m 5 -p 4.0 -t 10
```

Questo comando esegue l'algoritmo utilizzando il file `input.csv` come input, salva i risultati nel file `output.csv`, e utilizza i valori di default per i parametri opzionali non specificati, con `mu = 5`, `xi = 4.0`, e `tau = 10`.

## 3. Generazione del Dataset

Per generare un dataset, è possibile utilizzare lo script Bash `./script/generate.sh`. Questo script richiede la specifica di quattro parametri obbligatori: `-w`, `-n`, `-p`, e `-f`.

### 3.1 Sintassi del comando

```
./script/generate.sh -w 10 -n 5 -p 50 -f dataset.csv
```

### 3.2 Parametri

- `-w`: Indica la dimensione della finestra temporale (ad esempio, 10 giorni).
- `-n`: Specifica il numero di cluster da generare (ad esempio, 5).
- `-p`: Definisce il numero di punti per ciascun cluster (ad esempio, 50).
- `-f`: Specifica il percorso del file di output (ad esempio, `dataset.csv`).

## 4. Benchmark

Per eseguire i benchmark, vengono forniti diversi script. Ogni script esegue una parte specifica della valutazione delle performance.

### 4.1 Benchmark con valori di default

Per eseguire il benchmark (temporale e qualità) con i parametri di default, eseguire il comando:

```
./bench.sh
```

### 4.2 Analisi delle performance temporali con valori personalizzati

Per analizzare le performance temporali utilizzando valori personalizzati, utilizzare il comando:

```
./scripts/time_evaluation.sh
```

### 4.3 Ricerca dei parametri ottimali

Per individuare i parametri ottimali, eseguire il comando:

```
./scripts/parameters_evaluation.sh
```

## 5. Valutazione delle metriche esterne

Per calcolare le metriche esterne di clustering (Silhouette Score e BetaCV), utilizzare lo script Python `evaluate.py`.

### 5.1 Sintassi del comando

```
python3 evaluate.py input.csv output.csv
```

In questo comando, `input.csv` è il file contenente i dati da valutare, e `output.csv` è il file in cui verranno salvati i risultati.
