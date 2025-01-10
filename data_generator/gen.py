#!/usr/bin/env python3
import random

def generate_data(NUM_CLUSTERS=1000, points_per_cluster=5000, output_path="data.csv",
                  z=0.0010, min_dist=0.0050, seed=0, noise_ratio=0.01):
    random.seed(seed)  # for reproducibility

    centers = []
    all_points = []

    # Determine cluster centers
    while len(centers) < NUM_CLUSTERS:
        x = random.uniform(-180.0, +180.0)  # longitude
        y = random.uniform(-90.0, +90.0)  # latitude

        valid = True
        for (a, b) in centers:
            # ensure cluster centers are not too closely placed
            if abs(x - a) < min_dist or abs(y - b) < min_dist:
                valid = False
                break

        if valid:
            centers.append((x, y))
            if len(centers) % 1000 == 0:
                print("Centers created:", len(centers))

    print("Cluster centers determined.")

    # Spread points around centers (+/- z) and gradually add noise
    count = 0
    for (x, y) in centers:
        size = points_per_cluster
        if noise_ratio > 0:
            noise_interval = int(1 / noise_ratio)

        for i in range(size):
            # Generate a point around the cluster center
            eps = random.uniform(0.0, z)
            p1 = random.uniform(x - eps, x + eps)
            p2 = random.uniform(y - eps, y + eps)
            all_points.append((p1, p2))
            count += 1

            # Add noise gradually based on noise_interval
            if noise_ratio > 0 and i % noise_interval == 0:
                noise_x = random.uniform(-180.0, +180.0)  # longitude
                noise_y = random.uniform(-90.0, +90.0)    # latitude
                all_points.append((noise_x, noise_y))
                count += 1

            if count % 10000 == 0:
                print("Points created:", count)

    # Write data to the output file
    with open(output_path, "w") as f:
        for (x, y) in all_points:
            f.write(f"{x},{y}\n")


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Generate clustered data with gradual noise.")
    parser.add_argument("-n", "--cluster_number", type=int, default=1000, help="Number of clusters to generate (default: 1000).")
    parser.add_argument("-o", "--output_path", type=str, default="data.csv", help="Path for the output CSV file.")
    parser.add_argument("-z", "--spread", type=float, default=0.0010, help="Spread factor for points (default: 0.0010).")
    parser.add_argument("-d", "--min_distance", type=float, default=0.0050, help="Minimum distance between cluster centers.")
    parser.add_argument("-p", "--points_per_cluster", type=int, default=5000, help="Number of points per cluster.")
    parser.add_argument("-s", "--seed", type=int, default=0, help="Seed for RNG (default: 0).")
    parser.add_argument("--noise_ratio", type=float, default=0, help="Ratio of noise points to total points.")

    args = parser.parse_args()

    generate_data(
        NUM_CLUSTERS=args.cluster_number,
        points_per_cluster=args.points_per_cluster,
        output_path=args.output_path,
        z=args.spread,
        min_dist=args.min_distance,
        seed=args.seed,
        noise_ratio=args.noise_ratio
    )
