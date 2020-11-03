# Spinlocks

This repository contains a collection of spinlock implementations with various optimizations. This includes a basic (naive) spinlock, and ones with the following optimizations:

- Spinning locally
- Spinning with active backoff (`for` loop)
- Spinning with passive backoff (`_mm_pause()` intrinsic)
- Spinning with exponential backoff
- Spinning with randomized backoff
- Ticket-based spinlock

The benchmark used to evaluate the performance (among other metrics) of our spinlock is a simple loop where each thread increments a shared variable after grabbing the lock. This provides an extreme high-contention (but simple) scenario where we can evaluate out lock.

We also have benchmarks for the `pthread_mutex_t`, and `pthread_spinlock_t`, to compare performance and assembly against state-of-the-art locking mechanisms.
