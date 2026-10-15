# Task 04 – Matrix Addition

**Goal:** move from 1D arrays to 2D and compare parallelising only the outer loop with `collapse(2)`.

Given `N x N` matrices `A` and `B`, compute `C[i][j] = A[i][j] + B[i][j]`.

---

## Implementation

| File | Contents |
|---|---|
| `source/matadd.cpp` | `matAddSerial`, `matAddOuter` (`#pragma omp parallel for` on `i`), `matAddCollapse` (`#pragma omp parallel for collapse(2)`), verification, timing driver |

- **Storage:** flattened 1D row-major arrays, element `(i, j)` at index `i*N + j` (the lab sheet asks for this to be stated explicitly).
- Input values (`k % 13` and `(k % 5) * 0.25`) are exactly representable, so both parallel results are compared with the serial result **exactly**.
- **Sizes:** `N = 8` (the "tall"/few-iterations case, only 8 outer iterations) and `N = 4096`. Thread counts `1, 2, 4, 8`.
- **Timing small matrices:** an 8 x 8 addition takes tens of nanoseconds, far below timer resolution. The program therefore repeats the operation `reps = max(1, 4·10^6 / N²)` times inside each timed run (62 500 calls for `N = 8`, one call for `N = 4096`), and reports the **time per call in microseconds**. For the parallel versions this includes the cost of entering a parallel region on every call, which is exactly the overhead the `N = 8` experiment is about.

## Build and run

```bash
make            # builds bin/task4_matadd
make check      # N = 8 and 256, threads 1,2,4
make bench      # N = 8 and 4096; writes output/results.md
./bin/task4_matadd --sizes 8,64,4096 --threads 1,2,4,8
```

---

## My machine

| Item | Value |
|---|---|
| CPU model | _fill in_ |
| Physical cores / logical processors | _fill in_ |
| Compiler | _fill in_ |
| OS | _fill in_ |

## Results

Paste the tables from `output/results.md` (one per N: serial, outer-loop-only and `collapse(2)` times per call, both speedups, verification flag).

_Results to be pasted after running `make bench`._

## Observed behaviour (2–3 lines)

_Fill in: whether `collapse(2)` changed anything for `N = 8`, and for `N = 4096`._

---

## Questions

### Small N (N = 8): does `collapse(2)` help? Explain in terms of available independent iterations.

Parallelising only the outer loop exposes **N = 8 independent iterations** (one row each). With 8 threads that is one row per thread; with more threads than 8 some threads get nothing; with a thread count that does not divide 8 (for example 3, 5, 6) some threads get more rows than others, so the load is imbalanced. `collapse(2)` merges the `i` and `j` loops into one iteration space of `N × N = 64` independent iterations, which can be split far more evenly among threads. So `collapse(2)` improves the **balance and the number of schedulable iterations**, and it matters most when the thread count is close to or above N. In absolute time, however, the whole job is only 64 additions, so the fixed cost of starting a parallel region dominates both versions and neither is expected to beat the serial loop; any difference between the two parallel versions is small compared with that overhead (and `collapse` adds a little index-recovery arithmetic). _(Observed on my machine: fill in.)_

### Large N (N = 4096): does `collapse(2)` still make a measurable difference?

Not expected to. With 4096 outer iterations there are hundreds of times more rows than threads, so parallelising the outer loop alone already keeps every thread busy and the load is balanced to within one row. `collapse(2)` then only adds index arithmetic. Matrix addition is also memory-bandwidth bound (two loads and one store per add), so both versions run into the same memory limit and should scale about equally. _(Observed on my machine: fill in.)_
