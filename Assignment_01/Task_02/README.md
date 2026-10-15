# Task 02 – Sum of Array Elements

**Goal:** learn the `reduction` clause and see why a naive parallel sum is unsafe.

Given an array `a[N]` of `long`, compute the sum of all elements.

---

## Implementation

| File | Contents |
|---|---|
| `source/arraysum.cpp` | `sumSerial`, `sumNaiveParallelBroken` (intentional data race), `sumReduction` (`reduction(+:sum)`), `sumCritical` (`#pragma omp critical`), race demonstration, timing driver |

- Input values are `a[i] = i % 100`, so the exact sum (4.95 × 10^9 at `N = 10^8`) fits in a `long` and is the same for every correct method.
- **Race demonstration (requirement 1):** for every N and every thread count, the broken version is run **10 times** and the output lists how many runs were wrong, how many distinct results appeared, and the smallest and largest result.
- **Reduction check (requirement 3):** the reduction result is compared with the serial sum on **every** timed run, not just once; a mismatch aborts the program.
- **Critical version:** correct but slow, so it is only run for `N <= --critical-max` (default `10^7`) and with a single repetition. Pass `--critical-max 100000000` to measure it at `10^8` as well (expect it to take a long time).
- Default sizes: `N = 10^7, 10^8`. Default thread counts: `1, 2, 4, 8`.

## Build and run

```bash
make            # builds bin/task2_arraysum
make check      # N = 10^6, threads 1,2,4
make bench      # N = 10^7 and 10^8; writes output/results.md
```

---

## My machine

| Item | Value |
|---|---|
| CPU model | _fill in_ |
| Physical cores / logical processors | _fill in_ |
| Compiler | _fill in (`g++ --version`)_ |
| OS | _fill in_ |

## Results

Paste the output of `make bench` from `output/results.md`: for each N, the race table (wrong / distinct results of the naive version) and the timing table (serial, reduction, reduction speedup, critical).

_Results to be pasted after running `make bench`._

## Observed behaviour (2–3 lines)

_Fill in: how the naive version misbehaved (how many wrong runs, how far off), how the reduction speedup scaled, and how slow `critical` was compared with serial and with `reduction`._

---

## Questions

### Why does the naive version give wrong or inconsistent answers? (data race on `sum`)

`sum += a[i]` is really three steps: load `sum`, add `a[i]`, store `sum`. With the loop split across threads, all threads share the single variable `sum`. Two threads can load the same old value, each add their own element, and each store back; one of the two updates is overwritten and lost. Which updates are lost depends on how the threads happen to interleave, so the result changes from run to run and is normally smaller than the true sum. The compiler is also free to keep `sum` in a register during a thread's chunk and write it back only at the end, which makes the loss bigger (a thread's whole partial sum can be overwritten). The 1-thread row is always correct because there is nothing to race with; the wrong results appear from 2 threads upwards, and may need several runs to show up on a lightly loaded machine, which is why the race table repeats the run 10 times.

### Would `#pragma omp critical` around `sum += a[i]` fix correctness? What happens to performance versus `reduction`?

Yes, it fixes correctness: `critical` allows only one thread at a time to execute the update, so no update is lost. But it turns the loop into a serial one with extra cost: every iteration has to acquire and release the lock, and all threads queue on the same lock and bounce the cache line holding `sum`. Expect the critical version to be **slower than the plain serial loop** and to get no faster (often slower) as threads are added. `reduction(+:sum)` avoids this because each thread accumulates into its own private copy of `sum` with no synchronisation and the copies are combined once, when the loop finishes. Compare the `Critical (s)` and `Reduction (s)` columns of your table. _(Observed on my machine: fill in.)_
