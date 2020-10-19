# Spinlocks

This repository contains various C++ implementations of spinlocks with various optimizations. This includes:

- Spinning locally on the lock
- Spinning with constant backoff
- Spinning with exponential backoff

The benchmark used to evaluate the performance (among other metrics) of our spinlock is a simple loop where each thread increments a shared variable after grabbing the lock. This provides an extreme high-contention (but simple) scenario where we can evaluate out lock.

