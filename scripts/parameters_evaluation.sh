#!/bin/bash

input_folder="input"
mkdir -p "$input_folder"

output_folder="parameter_output"
mkdir -p "$output_folder"

show_help() {
  echo "Uso: $0 [-n numero_cluster] [-p punti_per_cluster] [-w finestra] [-m mu_lb] [-M mu_ub] [-x xi_lb] [-X xi_ub] [-t tau_lb] [-T tau_ub] [-d delta_lb] [-D delta_ub] [--help]"
  echo "  -n numero_cluster       Numero di cluster"
  echo "  -p punti_per_cluster    Numero di punti per cluster"
  echo "  -w finestra             Dimensione finestra temporale"
  echo "  -m mu_lb                Lower bound for mu (default: 2)"
  echo "  -M mu_ub                Upper bound for mu (default: 1000)"
  echo "  -x xi_lb                Lower bound for xi (default: 2)"
  echo "  -X xi_ub                Upper bound for xi (default: 3.5)"
  echo "  -t tau_lb               Lower bound for tau (default: 2)"
  echo "  -T tau_ub               Upper bound for tau (default: 1000)"
  echo "  -d delta_lb             Lower bound for delta (default: 1)"
  echo "  -D delta_ub             Upper bound for delta (default: 5)"
  echo "  --help                  Mostra questo messaggio di aiuto"
}

# Variabili di default
n=""
p=""
w=""
mu_lb="2"
mu_ub="1000"
xi_lb="2"
xi_ub="3.5"
tau_lb="2"
tau_ub="1000"
delta_lb="1"
delta_ub="5"

# Parsing degli argomenti
while getopts "n:p:w:m:M:x:X:t:T:d:D:h" opt; do
  case $opt in
    n) n="$OPTARG"
    ;;
    p) p="$OPTARG"
    ;;
    w) w="$OPTARG"
    ;;
    m) mu_lb="$OPTARG"
    ;;
    M) mu_ub="$OPTARG"
    ;;
    x) xi_lb="$OPTARG"
    ;;
    X) xi_ub="$OPTARG"
    ;;
    t) tau_lb="$OPTARG"
    ;;
    T) tau_ub="$OPTARG"
    ;;
    d) delta_lb="$OPTARG"
    ;;
    D) delta_ub="$OPTARG"
    ;;
    h) show_help
       exit 0
    ;;
    \?) echo "Invalid option -$OPTARG" >&2
        show_help
        exit 1
    ;;
  esac
done

# Verifica degli argomenti richiesti, controlla se vuoti
if [[ -z $n || -z $w || -z $p ]]; then
  echo "Errore: i primi 3 argomenti sono obbligatori."
  show_help
  exit 1
fi

# Print the parameters being used
echo "Using parameters:"
echo "numero_cluster: $n"
echo "punti_per_cluster: $p"
echo "finestra: $w"
echo "mu-lb: $mu_lb"
echo "mu-ub: $mu_ub"
echo "xi-lb: $xi_lb"
echo "xi-ub: $xi_ub"
echo "tau-lb: $tau_lb"
echo "tau-ub: $tau_ub"
echo "delta-lb: $delta_lb"
echo "delta-ub: $delta_ub"

# Generate data
timestamp=$(date +%s)
total_points=$(($p * $n))
points_per_day=$(($total_points / $w))

path="${n}_${p}"
mkdir -p "${input_folder}/${path}"
mkdir -p "${output_folder}/${path}"

result_filename="${output_folder}/${path}/${timestamp}_result.csv"
output_filename="${output_folder}/${path}/${timestamp}_output.csv"

# Run the Python scripts
input_filename="${input_folder}/${path}/${timestamp}.csv"
python3 data_generator/gen.py -n $n -p $p -s $timestamp -o $input_filename
python3 data_generator/change_label.py $input_filename $points_per_day

python3 scripts/optimizer.py $n $input_filename $output_filename $result_filename --mu-lb "$mu_lb" --mu-ub "$mu_ub" --xi-lb "$xi_lb" --xi-ub "$xi_ub" --tau-lb "$tau_lb" --tau-ub "$tau_ub" --delta-lb "$delta_lb" --delta-ub "$delta_ub"

