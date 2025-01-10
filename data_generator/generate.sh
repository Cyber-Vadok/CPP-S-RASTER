#!/bin/bash

window=10

# Array dei numeri di cluster
clusters=(10 100 1000)

# Array dei numeri di punti per cluster
points=(50 500 5000)

# Ciclo sugli indici degli array
for i in "${!clusters[@]}"; do
    n=${clusters[i]}  # Numero di cluster
    p=${points[i]}    # Numero di punti corrispondente
    echo "Cluster: $n, Punti: $p"
    path="input/n${n}_p${p}"
    mkdir -p $path
    total_points=$(($p*$n))
    points_per_day=$(($total_points/$window))
    for i in {1..10} #copie per combinazione parametri
    do
        python3 data_generator/gen.py -n $n -p $p -s $i -o "${path}/${i}.csv"
        python3 data_generator/change_label.py "${path}/${i}.csv" $points_per_day 
    done
done