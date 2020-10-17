// This program benchmarks pthread spinlock C++
// By: Nick from CoffeeBeforeArch

#include <benchmark/benchmark.h>
#include <pthread.h>

#include <atomic>
#include <cstdint>
#include <thread>

// Increment val once each time the lock is acquired
void inc_small(pthread_spinlock_t &sl, std::int64_t &val) {
  for (int i = 0; i < 100000; i++) {
    pthread_spin_lock(&sl);
    val++;
    pthread_spin_unlock(&sl);
  }
}

// Increment val 100 times each time the lock is acquired
void inc_medium(pthread_spinlock_t &sl, std::int64_t &val) {
  for (int i = 0; i < 1000; i++) {
    pthread_spin_lock(&sl);
    for (int j = 0; j < 100; j++) benchmark::DoNotOptimize(val++);
    pthread_spin_unlock(&sl);
  }
}

// Increment val 1000 times each time the lock is acquired
void inc_large(pthread_spinlock_t &sl, std::int64_t &val) {
  for (int i = 0; i < 100; i++) {
    pthread_spin_lock(&sl);
    for (int j = 0; j < 1000; j++) benchmark::DoNotOptimize(val++);
    pthread_spin_unlock(&sl);
  }
}

// Benchmark Small
static void pthread_spinlock_small(benchmark::State &s) {
  // Sweep over a range of threads
  auto num_threads = s.range(0);

  // Value we will increment
  std::int64_t val = 0;

  // Allocate a vector of threads
  std::vector<std::thread> threads;
  threads.reserve(num_threads);

  // Create a spinlock
  pthread_spinlock_t sl;
  pthread_spin_init(&sl, PTHREAD_PROCESS_PRIVATE);

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
BENCHMARK(pthread_spinlock_small)
    ->DenseRange(1, std::thread::hardware_concurrency())
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

// Benchmark Medium
static void pthread_spinlock_medium(benchmark::State &s) {
  // Sweep over a range of threads
  auto num_threads = s.range(0);

  // Value we will increment
  std::int64_t val = 0;

  // Allocate a vector of threads
  std::vector<std::thread> threads;
  threads.reserve(num_threads);

  // Create a spinlock
  pthread_spinlock_t sl;
  pthread_spin_init(&sl, PTHREAD_PROCESS_PRIVATE);

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
BENCHMARK(pthread_spinlock_medium)
    ->DenseRange(1, std::thread::hardware_concurrency())
    ->UseRealTime()
    ->Unit(benchmark::kMicrosecond);

// Benchmark Large
static void pthread_spinlock_large(benchmark::State &s) {
  // Sweep over a range of threads
  auto num_threads = s.range(0);

  // Value we will increment
  std::int64_t val = 0;

  // Allocate a vector of threads
  std::vector<std::thread> threads;
  threads.reserve(num_threads);

  // Create a spinlock
  pthread_spinlock_t sl;
  pthread_spin_init(&sl, PTHREAD_PROCESS_PRIVATE);

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
BENCHMARK(pthread_spinlock_large)
    ->DenseRange(1, std::thread::hardware_concurrency())
    ->UseRealTime()
    ->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
