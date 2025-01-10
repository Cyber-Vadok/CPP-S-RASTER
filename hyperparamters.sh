mu=1
tau=1
delta=1

for param in {1..10}
    do
        output_file="data_output/results_mu${mu}_tau${tau}_delta${delta}.csv"
        ./build/bin/sraster -m $param -t $tau -d $delta -o $output_file -f data_generator/data_1000.csv
    done
