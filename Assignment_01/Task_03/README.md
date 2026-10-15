# Task 03 – Maximum and Minimum of an Array

**Goal:** extend the reduction idea to `max`/`min`, and find not just the value but also its index.

Given an array `a[N]` of `int`, find the maximum value and the index where it occurs, then repeat for the minimum.

---

## Implementation

| File | Contents |
|---|---|
| `source/maxmin.cpp` | serial, `critical` and manual per-thread versions (templated over a max/min policy), `reduction(max:)` / `reduction(min:)` value-only versions, verification, timing driver |

Versions implemented for **both** maximum and minimum:

1. **Serial** – reference result (value and index).
2. **`reduction(max: maxVal)` / `reduction(min: minVal)`** – value only (requirement 1). Needs OpenMP 4.0+; GCC 9+ supports it.
3. **`critical` section** – a shared `(best_val, best_idx)` pair updated inside `#pragma omp critical` on every iteration, as in the sample on the lab sheet (requirement 2, first option).
4. **Manual per-thread reduction** – each thread scans its own contiguous chunk (bounds from `omp_get_thread_num()` / `omp_get_num_threads()`), keeps a local `(value, index)`, stores it in a per-thread slot, and a final serial pass over the slots picks the overall best (requirement 2, second option). The slots are padded to a cache line (`alignas(64)`) so neighbouring threads do not falsely share a line.

**Ties:** the test data deliberately contains the maximum value twice and the minimum value twice. All index-finding versions return the **first occurrence**, exactly like the serial version: the critical version breaks ties with the lower index, and the manual version scans chunks in index order and only replaces on a strictly better value. Verification compares both the value and the index.

- Data: pseudo-random `int`s in `[0, 10^9]` (fixed seed) with the planted maximum `2·10^9` and minimum `-5`.
- Default sizes: `N = 10^7, 10^8`; default threads `1, 2, 4, 8`.
- The `critical` versions run only for `N <= --critical-max` (default `10^7`) and with one repetition, because a lock per element is very slow at `10^8`.

## Build and run

```bash
make            # builds bin/task3_maxmin
make check      # N = 10^5, threads 1,2,4
make bench      # N = 10^7 and 10^8; writes output/results.md
```

---

## My machine

| Item | Value |
|---|---|
| CPU model | _fill in_ |
| Physical cores / logical processors | _fill in_ |
| Compiler / OpenMP version | _fill in_ |
| OS | _fill in_ |

## Results

Paste the tables from `output/results.md`: for each N, one table for the maximum and one for the minimum (serial, `reduction` value-only, `critical`, manual per-thread, manual speedup, correctness flag).

_Results to be pasted after running `make bench`._

## Observed behaviour (2–3 lines)

_Fill in: how the critical and manual approaches compare in your table, and how the manual approach scaled._

---

## Questions

### Why can't you write `reduction(max: best_idx)` to get the index of the maximum?

A `reduction(max: x)` clause keeps a private copy of one scalar per thread and combines the copies with the `max` operator. The index is not independent of the value: the index you want is the one that belongs to the winning value. With `reduction(max: best_idx)` each thread would just track the largest index it touched, and the combination would return the largest index seen by any thread (essentially a position near the end of the array), which has nothing to do with where the maximum value sits. A value and its index have to be updated together as one unit, and the built-in operators can only combine a single scalar. The ways around it are the ones used here: a `critical` section that updates the pair atomically, a manual per-thread `(value, index)` pair combined at the end, or (not implemented here) a user-defined reduction over a struct with `#pragma omp declare reduction`.

### `critical` approach vs manual per-thread reduction: which is faster, and why?

The manual per-thread approach is expected to be much faster. The `critical` version enters a lock-protected region for **every element**, so all threads serialise on one lock and the cache lines holding `best_val`/`best_idx` bounce between cores; it is typically slower than the serial loop and does not improve with more threads. In the manual version each thread runs an independent, synchronisation-free scan over about `N / threads` elements, and the only serial work is combining one result per thread at the end, so it behaves like the memory-bound scan it is and scales until memory bandwidth is the limit. Compare the `critical` and `manual` columns of your table. _(Observed on my machine: fill in.)_
