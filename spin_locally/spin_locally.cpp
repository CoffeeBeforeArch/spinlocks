// This program benchmarks an improved spinlock C++
// Optimizations:
//  1.) Spin locally
// By: Nick from CoffeeBeforeArch

#include <benchmark/benchmark.h>

#include <atomic>
#include <cstdint>
#include <thread>

// Simple Spinlock
// Lock now performs local spinning
struct Spinlock {
  // Lock is just an atomic bool
  std::atomic<bool> locked{false};

  // Locking mechanism
  void lock() {
    // Keep trying
    while (1) {
      // Try and grab the lock
      // Return if we get the lock
      if (!locked.exchange(true)) return;

      // If we didn't get the lock, just read the value which gets cached
      // locally This leads to less traffic
      while (locked.load())
        ;
    }
  }

  // Unlocking mechanism
  // Just set the lock to free (0)
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
static void spin_locally(benchmark::State &s) {
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
BENCHMARK(spin_locally)
    ->DenseRange(1, std::thread::hardware_concurrency())
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
