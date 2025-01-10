#!/bin/bash

# Array dei numeri di cluster
clusters=(10 100 1000)

# Array dei numeri di punti per cluster
points=(50 500 5000)

for i in "${!clusters[@]}"; do
    n=${clusters[i]}  # Numero di cluster
    p=${points[i]}    # Numero di punti corrispondente
    output_path="data_output/n${n}_p${p}"
    input_path="input/n${n}_p${p}"
    mkdir -p $output_path

    for mu in {1..10}
    do
        for tau in {1..10}
        do
        output_file="${output_path}/mu${mu}_tau${tau}.csv"
        input_file="${input_path}/1.csv"
        ./build/bin/sraster -m $mu -t $tau -f ${input_file} -o ${output_file}
        done
    done
done