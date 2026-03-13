#!/usr/bin/env python3

import csv
import sys

import matplotlib.pyplot as plt


def main():
    path = "data/results.csv"
    if len(sys.argv) == 2:
        path = sys.argv[1]

    pages = []
    times = []

    with open(path, "r", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in reader:
            pages.append(int(row["pages"]))
            times.append(float(row["time_ns_per_access"]))

    plt.figure(figsize=(10, 6))
    plt.plot(pages, times, marker="o")
    plt.xlabel("Number of pages")
    plt.ylabel("Time per access, ns")
    plt.title("TLB benchmark")
    plt.grid(True)
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    main()