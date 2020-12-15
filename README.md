# Spinlocks

This repository contains a collection of spinlock implementations with various optimizations. This includes a basic (naive) spinlock, and ones with the following optimizations:

- Spinning locally
  - Addresses constant contention from writes
- Spinning with active backoff (`for` loop)
  - Addresses bursty contention when the lock is released
- Spinning with passive backoff (`_mm_pause()` intrinsic)
  - Addresses bursty contention with the lock is released and saves on power
- Spinning with exponential backoff
  - Addresses bursty contention and power while accounting for non-uniform wait times of threads
- Spinning with randomized backoff
  - Addresses bursty contention and power while accounting for non-uniform wait times of threads
- Ticket-based spinlock
  - Addresses unfairness from previous implementations

The benchmark used to evaluate the performance (among other metrics) of our spinlock is a simple loop where each thread increments a shared variable after grabbing the lock. This provides an extreme high-contention (but simple) scenario where we can evaluate out lock.

We also have benchmarks for the `pthread_mutex_t`, and `pthread_spinlock_t`, to compare performance and assembly against state-of-the-art locking mechanisms.
