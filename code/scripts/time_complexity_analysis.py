import subprocess
import time
import numpy as np
import matplotlib.pyplot as plt
from typing import List, Tuple
import json

class NumpyEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, np.integer):
            return int(obj)
        if isinstance(obj, np.floating):
            return float(obj)
        if isinstance(obj, np.ndarray):
            return obj.tolist()
        return super(NumpyEncoder, self).default(obj)

def generate_and_run(n: int) -> float:
    """Generates a test case of size n and returns execution time in seconds"""
    # Generate test case
    gen_process = subprocess.Popen(
        ["../scripts/test_generator", str(n)],
        stdout=subprocess.PIPE
    )
    
    # Run main.py and measure time
    start_time = time.time()
    process = subprocess.Popen(
        ["python3.13", "../src_python/main.py"],
        stdin=gen_process.stdout,
        stdout=subprocess.DEVNULL
    )
    process.wait()
    end_time = time.time()
    
    return end_time - start_time

def analyze_complexity(start_n: int = 6, end_n: int = 100, steps: int = 40) -> Tuple[List[int], List[float]]:
    """Run tests for different sizes and collect timing data"""
    # Try to load existing results
    try:
        with open('complexity_results.json', 'r') as f:
            data = json.load(f)
            print("Loaded existing results from file")
            return data['sizes'], data['times']
    except FileNotFoundError:
        sizes = np.linspace(start_n, end_n, steps, dtype=int)
        times = []
        
        for n in sizes:
            trials = [generate_and_run(n) for _ in range(3)]
            avg_time = sum(trials) / len(trials)
            times.append(avg_time)
            print(f"n={n}: {avg_time:.3f} seconds")
        
        # Save results to file
        sizes = [int(x) for x in sizes]
        times = [float(x) for x in times]
        with open('complexity_results.json', 'w') as f:
            json.dump({'sizes': sizes, 'times': times}, f, cls=NumpyEncoder)
        
        return list(sizes), times

def plot_results(sizes: List[int], times: List[float]):
    """Plot results and n³ complexity curve"""
    plt.figure(figsize=(10, 6))
    
    # Plot actual data points
    plt.scatter(sizes, times, color='blue', label='Measured times')
    
    # Plot n³ curve
    x = np.array(sizes)
    # Scale the n³ curve to match the actual data
    scale_factor = np.mean(times / (x**3))
    plt.plot(x, scale_factor * x**3, 'r--', label='O(n³)')
    
    plt.xlabel('Input Size (n)')
    plt.ylabel('Time (seconds)')
    plt.title('Time Complexity Analysis')
    plt.legend()
    plt.grid(True)
    plt.savefig('../../latex/figuras/time_complexity.png')
    plt.close()

def main():
    print("Running time complexity analysis...")
    sizes, times = analyze_complexity()
    plot_results(sizes, times)
    print("\nAnalysis complete! Results saved to 'time_complexity.png'")

if __name__ == "__main__":
    main()
