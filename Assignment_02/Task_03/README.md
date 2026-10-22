# Task 03 – Histogram Calculation

**Goal:** given an array of integers from 0 to 9, count how often each number occurs using OpenMP, while preventing race conditions.

---

## Implementation

| File | Contents |
|---|---|
| `source/histogram.cpp` | `histSerial`, `histNaiveBroken` (intentional race), `histAtomic`, `histCritical`, `histReduction`, `histPrivate`, race demonstration, verification, timing driver |

| Version | How the race is handled |
|---|---|
| Serial | no parallelism, reference result |
| Naive parallel (broken) | `h[a[i]]++` on a shared array with no protection; kept only to demonstrate the race |
| Atomic | `#pragma omp atomic` on `h[a[i]]++`: each increment is an indivisible read-modify-write |
| Critical | `#pragma omp critical` around `h[a[i]]++`: one thread at a time may execute the increment |
| Reduction | `reduction(+ : local[:10])`: OpenMP gives every thread a private zeroed copy of the 10 counters and adds the copies together at the end (array-section reductions need OpenMP 4.5, GCC 6+) |
| Private + merge | each thread counts into its own local 10-element array inside a `parallel` region, then adds it into the shared histogram once, inside a named `critical` section |

- **Data:** random integers in `[0, 9]` (fixed seed).
- **Race demonstration:** for every N and thread count, the naive version is run **10 times**; the output lists how many histograms were wrong, how many different histograms appeared, and the smallest total number of elements counted (the correct total is `N`). On a lightly loaded or single-core machine the race may not show up; the program says so when no wrong result appears, because that does not make the unprotected code correct.
- **Verification:** every protected version is compared bin by bin with the serial histogram.
- **Sizes:** `N = 10^6, 10^7, 10^8`; threads `1, 2, 4, 8`. The `critical` version is only run for `N <= --critical-max` (default `10^7`) and once, because it is very slow.

## Build and run

```bash
make            # builds bin/task3_histogram
make check      # N = 10^6, threads 1,2,4
make bench      # default sizes; writes output/results.md
./bin/task3_histogram --sizes 10000000 --threads 1,2,4,8 --critical-max 10000000
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

Paste the tables from `output/results.md`: for each N, the race table and the timing table (serial, atomic, critical, reduction, private + merge).

_Results to be pasted after running `make bench`._

## Observed behaviour (2–3 lines)

_Fill in: the ranking of the four protected versions in your table and whether the race showed up on your machine._

---

## Preventing the race condition

**The problem.** `h[a[i]]++` is load, add, store on a counter that every thread may share. With only 10 counters and millions of increments, threads constantly target the same counter, so two threads can read the same old value and both store `old + 1`, losing one count. The total of the histogram then comes out below `N` and differs from run to run.

**Atomic and critical** make the update itself safe. `atomic` uses a single hardware read-modify-write instruction for the one memory update; `critical` uses a lock around a block of code. Both are correct but both **serialise the updates to the same counter**, and with only 10 counters the threads collide almost all the time (the cache line holding the counters also moves between cores on every increment). Expect `critical` to be the slowest, `atomic` to be faster than `critical`, and both to be slow compared with the serial loop at low thread counts. _(Observed on my machine: fill in.)_

**Reduction and private histograms** remove the sharing instead of protecting it. Each thread counts into its own 10 counters with no synchronisation at all, and only the final 10 additions per thread are combined (by OpenMP for `reduction`, by a short `critical` section for the manual version). The expensive part, the pass over `N` elements, is completely free of synchronisation, so these two versions are expected to scale and to beat atomic and critical by a wide margin. The thread-local arrays live on each thread's own stack, far apart in memory, so they do not suffer false sharing either.

**Rule of thumb illustrated here:** when many threads update a small number of shared locations, give each thread a private copy and merge once, instead of synchronising every update.
