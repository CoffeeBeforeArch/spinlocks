// This program benchmarks an improved spinlock C++
// Optimizations:
//  1.) Spin locally
//  2.) Backoff
// By: Nick from CoffeeBeforeArch

#include <benchmark/benchmark.h>
#include <emmintrin.h>

#include <atomic>
#include <cstdint>
#include <random>
#include <thread>
#include <vector>

// Simple Spinlock
// Lock now performs local spinning
// Lock now does backoff
class Spinlock {
 private:
  // Lock is just an atomic bool
  std::atomic<bool> locked{false};
  // Random number generator
  std::uniform_int_distribution<int> dist;
  std::mt19937 rng;

 public:
  // Constructor to set up our RNG
  Spinlock() {
    // Get random numbers between 4 and 1024
    rng.seed(std::random_device()());
    dist = std::uniform_int_distribution<int>(4, 1024);
  }

  // Locking mechanism
  void lock() {
    // Keep trying
    while (1) {
      // Try and grab the lock
      // Exit if we got the lock
      if (!locked.exchange(true)) return;

      // Get a random number of iterations between 4 and 1024
      int backoff = dist(rng);

      // If we didn't get the lock, just read the value which gets cached
      // locally. This leads to less traffic.
      // Pause for a random number of iterations (between 4 and 1024)
      do {
        // Pause for some number of iterations
        for (int i = 0; i < backoff; i++) _mm_pause();

        // Check to see if the lock is now free
      } while (locked.load());
    }
  }

  // Unlocking mechanism
  // Just set the lock to free (false)
  // Can also use the assignment operator
  void unlock() { locked.store(false); }
};

// Increment val once each time the lock is acquired
void inc(Spinlock &s, std::int64_t &val) {
  for (int i = 0; i < 100000; i++) {
    s.lock();
    val++;
    s.unlock();
  }
}

// Small Benchmark
static void random_backoff(benchmark::State &s) {
  // Sweep over a range of threads
  auto num_threads = s.range(0);

  // Value we will increment
  std::int64_t val = 0;

  // Allocate a vector of threads
  std::vector<std::thread> threads;
  threads.reserve(num_threads);

  Spinlock sl;

  // Timing loop
  for (auto _ : s) {
    for (auto i = 0u; i < num_threads; i++) {
      threads.emplace_back([&] { inc(sl, val); });
    }
    // Join threads
    for (auto &thread : threads) thread.join();
    threads.clear();
  }
}
BENCHMARK(random_backoff)
    ->RangeMultiplier(2)
    ->Range(1, std::thread::hardware_concurrency())
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
