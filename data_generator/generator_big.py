#!/usr/bin/env python3
import os
import random
import sys

def generate_data(NUM_CLUSTERS, output_folder=None):
    random.seed(0)  # for reproducibility

    # Clusters have 500 points that are randomly spread around a center
    points_cluster = 500

    centers = []
    all_points = []

    # Determine cluster centers
    while len(centers) < NUM_CLUSTERS:

        x = random.uniform(-180.0, +180.0)  # longitude
        y = random.uniform(-90.0, +90.0)  # latitude

        valid = True
        for (a, b) in centers:
            # ensure cluster centers are not too closely placed
            min_dist = 0.0050  # 50 * 1.1 m

            if abs(x - a) < min_dist or abs(y - b) < min_dist:
                valid = False
                break

        if valid:
            centers.append((x, y))
            if len(centers) % 1000 == 0:
                print("Centers created:", len(centers))

    print("Cluster centers determined.")

    # spread points around center (+/- z)
    count = 0
    for (x, y) in centers:

        # size = random.randint(500, 800)  # random size
        size = points_cluster  # fixed size

        for i in range(0, size):
            # spread is random, too
            z = random.uniform(0.0, 0.0010)
            eps = random.uniform(0.0, z)
            p1 = random.uniform(x - eps, x + eps)
            p2 = random.uniform(y - eps, y + eps)

            all_points.append((p1, p2))
            count += 1
            if count % 10000 == 0:
                print("Points created:", count)

    if output_folder:
        output_file = os.path.join(output_folder, "data_" + str(NUM_CLUSTERS) + ".csv")
    else:
        output_file = "data_" + str(NUM_CLUSTERS) + ".csv"

    with open(output_file, "w") as f:
        for (x, y) in all_points:
            f.write(str(x) + "," + str(y) + "\n")


if __name__ == "__main__":
    if len(sys.argv) == 2:
        output_folder = sys.argv[1]
    elif len(sys.argv) == 1:
        output_folder = None
    else:
        print("Usage: python script.py [output_folder]")
        sys.exit(1)

    # 1 to 10,000 centers
    for i in range(0, 5):
        generate_data(10 ** i, output_folder)
