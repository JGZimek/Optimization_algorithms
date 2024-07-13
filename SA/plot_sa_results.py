import matplotlib.pyplot as plt
import os
import sys

def plot_sa_results(directory):
    files = [f for f in os.listdir(directory) if f.startswith('results_sa_') and f.endswith('.txt')]
    for file in files:
        filepath = os.path.join(directory, file)
        with open(filepath, 'r') as f:
            data = [int(line.strip()) for line in f.readlines()]
        
        plt.figure()
        plt.plot(data)
        plt.title(f'Simulated Annealing Results for {file}')
        plt.xlabel('Iteration')
        plt.ylabel('C_max')
        plot_filename = os.path.join(directory, f"{file[:-4]}.png")
        plt.savefig(plot_filename)
        plt.close()

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 plot_sa_results.py <results_directory>")
        sys.exit(1)
    
    results_directory = sys.argv[1]
    plot_sa_results(results_directory)
