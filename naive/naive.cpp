// This program benchmarks a simple spinlock in C++
// By: Nick from CoffeeBeforeArch

#include <benchmark/benchmark.h>

#include <atomic>
#include <cstdint>
#include <thread>
#include <vector>

// Simple Spinlock
struct Spinlock {
  // Lock is just an atomic bool
  std::atomic<bool> locked{false};

  // Locking mechanism
  void lock() {
    // Exchange will return the previous value of the lock
    // If the lock is free (false), it is set to true and the loop exits
    // If the lock is taken (true), spin in the loop until the loop exits
    while (locked.exchange(true))
      ;
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
static void naive(benchmark::State &s) {
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
BENCHMARK(naive)
    ->DenseRange(1, std::thread::hardware_concurrency())
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
