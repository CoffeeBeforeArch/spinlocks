// This program benchmarks an improved spinlock C++
// Optimizations:
//  1.) Spin locally
//  2.) Backoff
//  3.) Add exponential backoff
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

  // Constants for number of backoff iterations
  const int MAX_BACKOFF = 1 << 10;
  const int MIN_BACKOFF = 1;

  // Locking mechanism
  void lock() {
    // Start backoff at MIN_BACKOFF iterations
    int backoff = MIN_BACKOFF;

    // Keep trying
    while (1) {
      // Try and grab the lock
      // Return if we get the lock
      if (!locked.exchange(true)) return;

      // If we didn't get the lock, just read the value which gets cached
      // locally. This leads to less traffic.
      // Designed to improve the performance of spin-wait loops.
      while (locked.load()) {
        for (int i = 0; i < backoff; i++) __builtin_ia32_pause();
        backoff = std::min(backoff << 1, MAX_BACKOFF);
      }
    }
  }

  // Unlocking mechanism
  // Just set the lock to free (0)
  // Can also use the assignment operator
  void unlock() { locked.store(false, std::memory_order_release); }
};

// Increment val once each time the lock is acquired
void inc_small(Spinlock &s, std::int64_t &val) {
  for (int i = 0; i < 100000; i++) {
    s.lock();
    val++;
    s.unlock();
  }
}

// Increment val 100 times each time the lock is acquired
void inc_medium(Spinlock &s, std::int64_t &val) {
  for (int i = 0; i < 1000; i++) {
    s.lock();
    for (int j = 0; j < 100; j++) benchmark::DoNotOptimize(val++);
    s.unlock();
  }
}

// Increment val 1000 times each time the lock is acquired
void inc_large(Spinlock &s, std::int64_t &val) {
  for (int i = 0; i < 100; i++) {
    s.lock();
    for (int j = 0; j < 1000; j++) benchmark::DoNotOptimize(val++);
    s.unlock();
  }
}

// Small Benchmark
static void exp_backoff_small(benchmark::State &s) {
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
      threads.emplace_back([&] { inc_small(sl, val); });
    }
    // Join threads
    for (auto &thread : threads) thread.join();
    threads.clear();
  }
}
BENCHMARK(exp_backoff_small)
    ->DenseRange(1, std::thread::hardware_concurrency())
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

// Medium Benchmark
static void exp_backoff_medium(benchmark::State &s) {
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
      threads.emplace_back([&] { inc_medium(sl, val); });
    }
    // Join threads
    for (auto &thread : threads) thread.join();
    threads.clear();
  }
}
BENCHMARK(exp_backoff_medium)
    ->DenseRange(1, std::thread::hardware_concurrency())
    ->UseRealTime()
    ->Unit(benchmark::kMicrosecond);

// Large Benchmark
static void exp_backoff_large(benchmark::State &s) {
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
      threads.emplace_back([&] { inc_large(sl, val); });
    }
    // Join threads
    for (auto &thread : threads) thread.join();
    threads.clear();
  }
}
BENCHMARK(exp_backoff_large)
    ->DenseRange(1, std::thread::hardware_concurrency())
    ->UseRealTime()
    ->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
