// This program benchmarks std::mutex C++
// By: Nick from CoffeeBeforeArch

#include <benchmark/benchmark.h>

#include <atomic>
#include <cstdint>
#include <mutex>
#include <thread>

// Increment val once each time the lock is acquired
void inc(std::mutex &m, std::int64_t &val) {
  for (int i = 0; i < 100000; i++) {
    m.lock();
    val++;
    m.unlock();
  }
}

// Benchmark Small
static void std_mutex(benchmark::State &s) {
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
      threads.emplace_back([&] { inc(m, val); });
    }
    // Join threads
    for (auto &thread : threads) thread.join();
    threads.clear();
  }
}
BENCHMARK(std_mutex)
    ->RangeMultiplier(2)
    ->Range(1, std::thread::hardware_concurrency())
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
