// This program benchmarks std::mutex C++
// By: Nick from CoffeeBeforeArch

#include <benchmark/benchmark.h>

#include <atomic>
#include <cstdint>
#include <mutex>
#include <thread>

// Increment val once each time the lock is acquired
void inc_small(std::mutex &m, std::int64_t &val) {
  for (int i = 0; i < 100000; i++) {
    m.lock();
    val++;
    m.unlock();
  }
}

// Increment val 100 times each time the lock is acquired
void inc_medium(std::mutex &m, std::int64_t &val) {
  for (int i = 0; i < 1000; i++) {
    m.lock();
    for (int j = 0; j < 100; j++) benchmark::DoNotOptimize(val++);
    m.unlock();
  }
}

// Increment val 1000 times each time the lock is acquired
void inc_large(std::mutex &m, std::int64_t &val) {
  for (int i = 0; i < 100; i++) {
    m.lock();
    for (int j = 0; j < 1000; j++) benchmark::DoNotOptimize(val++);
    m.unlock();
  }
}


// Benchmark Small
static void std_mutex_small(benchmark::State &s) {
  // Sweep over a range of threads
  auto num_threads = s.range(0);

  // Value we will increment
  std::int64_t val = 0;

  // Allocate a vector of threads
  std::vector<std::thread> threads;
  threads.reserve(num_threads);

  std::mutex m;

  // Timing loop
  for (auto _ : s) {
    for (auto i = 0u; i < num_threads; i++) {
      threads.emplace_back([&] { inc_small(m, val); });
    }
    // Join threads
    for (auto &thread : threads) thread.join();
    threads.clear();
  }
}
BENCHMARK(std_mutex_small)
    ->DenseRange(1, std::thread::hardware_concurrency())
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

// Medium Benchmark
static void std_mutex_medium(benchmark::State &s) {
  // Sweep over a range of threads
  auto num_threads = s.range(0);

  // Value we will increment
  std::int64_t val = 0;

  // Allocate a vector of threads
  std::vector<std::thread> threads;
  threads.reserve(num_threads);

  std::mutex m;

  // Timing loop
  for (auto _ : s) {
    for (auto i = 0u; i < num_threads; i++) {
      threads.emplace_back([&] { inc_medium(m, val); });
    }
    // Join threads
    for (auto &thread : threads) thread.join();
    threads.clear();
  }
}
BENCHMARK(std_mutex_medium)
    ->DenseRange(1, std::thread::hardware_concurrency())
    ->UseRealTime()
    ->Unit(benchmark::kMicrosecond);

// Large Benchmark
static void std_mutex_large(benchmark::State &s) {
  // Sweep over a range of threads
  auto num_threads = s.range(0);

  // Value we will increment
  std::int64_t val = 0;

  // Allocate a vector of threads
  std::vector<std::thread> threads;
  threads.reserve(num_threads);

  std::mutex m;

  // Timing loop
  for (auto _ : s) {
    for (auto i = 0u; i < num_threads; i++) {
      threads.emplace_back([&] { inc_large(m, val); });
    }
    // Join threads
    for (auto &thread : threads) thread.join();
    threads.clear();
  }
}
BENCHMARK(std_mutex_large)
    ->DenseRange(1, std::thread::hardware_concurrency())
    ->UseRealTime()
    ->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
