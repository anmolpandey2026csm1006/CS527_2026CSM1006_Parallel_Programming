# Task 01 – Array Search

**Goal:** search for a given element in an array using OpenMP, determine whether it exists and, if it does, find its index.

---

## Implementation

| File | Contents |
|---|---|
| `source/arraysearch.cpp` | `searchSerial`, `searchReductionMin`, `searchEarlyExit`, scenario setup, verification, timing driver |

| Version | How it works | Returns |
|---|---|---|
| Serial | linear scan, stops at the first match | the first index, or -1 |
| Reduction min | `#pragma omp parallel for reduction(min : first)`; every match updates a private minimum index, the private minima are combined with `min` | the **first** index, or -1; scans the whole array |
| Early exit | the array is cut into blocks of 4096; blocks are handed out with `schedule(dynamic)`; a shared `stop` flag (read and written with `omp atomic`) is checked before each block; the found index is updated inside a named `critical` section | **an** index where the target occurs (not necessarily the first), or -1; threads stop early once any thread finds the target |

- **Data:** pseudo-random ints in `[0, 999999999]` (fixed seed). The search target is `2000000000`, which never occurs in the background data, so the experiment controls exactly where (and whether) the target is present.
- **Three scenarios per N:** target near the start (index `N/100`), target in the middle (planted at `N/2` and again at `3N/4`, so the "first occurrence" question is real), target absent (worst case: the whole array must be scanned).
- **Verification:** the reduction result must equal the serial index exactly. The early-exit result must be -1 exactly when the serial result is -1; otherwise it must be an index `>=` the serial index at which the array really holds the target.
- **Sizes:** `N = 10^6, 10^7, 10^8` by default; threads `1, 2, 4, 8`.

## Build and run

```bash
make            # builds bin/task1_arraysearch
make check      # N = 10^6, threads 1,2,4
make bench      # N = 10^6, 10^7, 10^8; writes output/results.md
./bin/task1_arraysearch --sizes 10000000 --threads 1,2,4,8 --repeats 5
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

Paste the tables from `output/results.md` (for each N, one table per scenario: serial, reduction and early-exit times, both speedups, correctness flag).

_Results to be pasted after running `make bench`._

## Observed behaviour (2–3 lines)

_Fill in: which version won in each scenario and whether more threads helped._

---

## Design notes

**Why `reduction(min)` gives the first occurrence deterministically.** Each thread keeps a private minimum of the matching indices it saw, and the combination step takes the smallest of those, so the result is the smallest matching index in the whole array no matter how the iterations were divided. A plain shared "found = i" assignment would instead return whichever thread wrote last, which varies from run to run.

**Cost of that guarantee.** A reduction loop cannot leave early, so this version always reads all `N` elements. When the target sits near the start, the serial loop finishes almost immediately and the reduction version is expected to be *slower than serial*; it only wins when the target is absent or far into the array (and even then it is limited by memory bandwidth, since the loop does almost no arithmetic).

**Early exit.** The early-exit version recovers the "stop when found" behaviour by testing a shared flag once per block of 4096 elements. Checking per block rather than per element keeps the atomic read cheap. The price is that it may return a different occurrence than the serial scan when the target appears more than once (any thread may find its copy first); the verification therefore accepts any real occurrence at or after the serial index. For an absent target it has nothing to stop on and behaves like the reduction version plus a little flag-checking overhead.

**Why not the OpenMP `cancel` construct.** Cancellation only takes effect if the `OMP_CANCELLATION=true` environment variable is set when the program starts, which cannot be arranged from inside the program. The shared-flag approach works with any default OpenMP environment.
