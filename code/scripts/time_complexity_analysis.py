import subprocess
import time
import numpy as np
import matplotlib.pyplot as plt
from typing import List, Tuple

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

def analyze_complexity(start_n: int = 6, end_n: int = 301, steps: int = 100) -> Tuple[List[int], List[float]]:
    """Run tests for different sizes and collect timing data"""
    sizes = np.linspace(start_n, end_n, steps, dtype=int)
    times = []
    
    for n in sizes:
        # Run 3 times and take average to reduce noise
        trials = [generate_and_run(n) for _ in range(3)]
        avg_time = sum(trials) / len(trials)
        times.append(avg_time)
        print(f"n={n}: {avg_time:.3f} seconds")
    
    return list(sizes), times

def plot_results(sizes: List[int], times: List[float]):
    """Plot results and try to fit different complexity curves"""
    plt.figure(figsize=(10, 6))
    
    # Plot actual data points
    plt.scatter(sizes, times, color='blue', label='Measured times')
    
    # Fit and plot different complexity curves
    x = np.array(sizes)
    
    # O(n!)
    fact_fit = np.polyfit(np.log(x), np.log(times), 1)
    plt.plot(x, np.exp(fact_fit[1]) * x**fact_fit[0], 
             'r--', label=f'O(n^{fact_fit[0]:.2f})')
    
    plt.xlabel('Input Size (n)')
    plt.ylabel('Time (seconds)')
    plt.title('Time Complexity Analysis')
    plt.legend()
    plt.grid(True)
    plt.savefig('time_complexity.png')
    plt.close()

def main():
    print("Running time complexity analysis...")
    sizes, times = analyze_complexity()
    plot_results(sizes, times)
    print("\nAnalysis complete! Results saved to 'time_complexity.png'")

if __name__ == "__main__":
    main()
