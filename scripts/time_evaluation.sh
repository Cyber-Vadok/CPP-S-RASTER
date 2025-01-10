#!/bin/bash

input_folder="input"
mkdir -p "$input_folder"

time_folder="time_output"
mkdir -p "$time_folder"


show_help() {
  echo "Uso: $0 [-n numero_cluster] [-p punti_per_cluster] [-w finestra] [-m trials] [--help]"
  echo "  -n numero_cluster       Numero di cluster"
  echo "  -p punti_per_cluster    Numero di punti per cluster"
  echo "  -w finestra             Dimensione finestra temporale"
  echo "  -m tentativi             trials"
  echo "  --help                  Mostra questo messaggio di aiuto"
}

# Variabili di default (se necessario)
n=""
p=""
w=""
m=""

# Parsing degli argomenti
while [[ "$1" != "" ]]; do
  case $1 in
    -n)
      shift
      n=$1
      ;;
    -p)
      shift
      p=$1
      ;;
    -w)
      shift
      w=$1
      ;;
    -m)
      shift
      m=$1
      ;;
    --help)
      show_help
      exit 0
      ;;
    *)
      echo "Opzione sconosciuta: $1"
      show_help
      exit 1
      ;;
  esac
  shift
done

# Verifica degli argomenti richiesti, controlla se vuoti
if [[ -z $n || -z $m || -z $w || -z $p ]]; then
  echo "Errore: tutti gli argomenti sono obbligatori."
  show_help
  exit 1
fi

# Generate data
timestamp=$(date +%s)
total_points=$(($p * $n))
points_per_day=$(($total_points / $w))

path="${n}_${p}"
mkdir -p "${input_folder}/${path}"
mkdir -p "${time_folder}/${path}"

output_filename="${time_folder}/${path}/${timestamp}.csv"

for (( i=1; i<=m; i++ ))
do
    # Run the Python scripts
    input_filename="${input_folder}/${path}/${timestamp}_${i}.csv"
    python3 data_generator/gen.py -n $n -p $p -s $i -o $input_filename
    python3 data_generator/change_label.py $input_filename $points_per_day
    ./build/bin/sraster -f "$input_filename" -c "$output_filename"
done

python3 time_output/plotter.py $output_filename