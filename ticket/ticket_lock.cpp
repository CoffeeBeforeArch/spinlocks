// This program benchmarks an improved spinlock C++
// Optimizations:
//  1.) Spin locally
//  2.) Backoff
//  3.) Ticket-based (for fairness)
// By: Nick from CoffeeBeforeArch

#include <benchmark/benchmark.h>
#include <emmintrin.h>

#include <atomic>
#include <cstdint>
#include <thread>
#include <vector>

// Simple Spinlock
// Lock now performs local spinning
// Lock now does backoff
class Spinlock {
 private:
  // Lock is now two counters:
  //  1.) The latest place taken in line
  //  2.) Which number is currently being served
  std::atomic<std::uint64_t> line{0};
  std::uint64_t serving{0};

 public:
  // Locking mechanism
  void lock() {
    // Get the latest place in line (and increment the value)
    auto place = line.fetch_add(1);

    // Wait until our number is "called"
    while (serving != place)
      ;
  }

  // Unlocking mechanism
  // Increment serving number to pass the lock
  // No need for an atomic! The thread with the lock is the only one that
  // accesses this variable!
  void unlock() { serving++; }
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
static void ticket_lock(benchmark::State &s) {
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
BENCHMARK(ticket_lock)
    ->RangeMultiplier(2)
    ->Range(1, std::thread::hardware_concurrency())
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
