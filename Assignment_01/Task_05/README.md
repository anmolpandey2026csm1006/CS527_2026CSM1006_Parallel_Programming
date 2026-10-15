# Task 05 – Matrix-Matrix Multiplication (Naive)

**Goal:** parallelise the classic triple-nested-loop multiplication and study how the choice of parallelised loop and `collapse` affects performance.

Given `N x N` matrices `A` and `B`, compute `C = A x B`.

---

## Implementation

| File | Contents |
|---|---|
| `source/matmul.cpp` | serial i-j-k, outer-`i` parallel, `collapse(2)` on `(i, j)`, bonus i-k-j serial and outer-`i` parallel, an intentionally broken parallel-`k` version for the race demonstration, tolerance-based verification, timing driver |

| Version | Loop structure |
|---|---|
| Serial | `i, j, k` |
| Outer parallel | `#pragma omp parallel for` on `i` |
| Collapse | `#pragma omp parallel for collapse(2)` on `i, j` (the `k` loop is **not** collapsed) |
| Bonus serial / parallel | `i, k, j` order (parallel version: `parallel for` on `i`) |
| Race demo only | `parallel for` on `k` with no reduction (deliberately wrong) |

- **Storage:** flattened 1D row-major arrays, element `(i, j)` at index `i*N + j`.
- **Local accumulator:** the i-j-k versions accumulate into a local `double sum` and write `C[i][j]` once, as the note on the lab sheet recommends. All three i-j-k versions do this, so the only difference between "outer" and "collapse" is which loops are distributed. `C` is zero-initialised before every i-k-j run (that order accumulates directly into `C`).
- **Verification:** each parallel result is compared with the serial result element by element with a relative tolerance of `1e-9` (floating-point matrices, requirement 3). A mismatch aborts with exit code 1.
- **Sizes and threads:** `N = 256, 512, 1024`; threads `1, 2, 4, 8`. Speedup and parallel efficiency (`speedup / threads`) are reported for every row.
- **Race demo:** before the timing tables the program runs the plain `parallel for` on the `k` loop at `N = 64` and prints how many elements of `C` differ from the serial result. A count of 0 does not mean the version is correct; the corruption only appears when threads really overlap, so run it on a multi-core machine.

## Build and run

```bash
make            # builds bin/task5_matmul
make check      # N = 64 and 128, threads 1,2,4
make bench      # N = 256, 512, 1024; writes output/results.md
./bin/task5_matmul --sizes 512,1024 --threads 1,2,4,8 --repeats 3 --no-race-demo
```

`N = 1024` performs about 2 × 10^9 floating-point operations per run, so the full benchmark with repeats and the bonus takes a while; use `--repeats 1` for a faster pass.

---

## My machine

| Item | Value |
|---|---|
| CPU model | _fill in_ |
| Physical cores / logical processors | _fill in_ |
| Compiler | _fill in_ |
| OS | _fill in_ |

## Results

Paste the tables from `output/results.md`: for each N, the i-j-k table (outer-`i` and `collapse(2)` times, speedup, efficiency) and the bonus i-k-j table. Also paste the race-demo line.

_Results to be pasted after running `make bench`._

## Observed behaviour (2–3 lines)

_Fill in: speedup/efficiency trend with thread count, whether `collapse(2)` made any difference, and what the i-k-j loop order did to the serial time and to scaling (bonus)._

---

## Questions

### Why is it safe to parallelise the `i` loop directly, even though the innermost `k` loop sums into `C[i][j]`?

Each iteration of the `i` loop writes only to row `i` of `C`, and `A` and `B` are only read. Different threads therefore never write the same element of `C`. The summation over `k` for a given `C[i][j]` runs entirely inside one thread, in order, in that thread's private `sum`. There is no shared variable that two threads update, so there is no race.

### What happens to correctness if the `k` loop is parallelised directly with a plain `parallel for` (no reduction)?

Then different threads execute different values of `k` for the **same** `(i, j)`, and they all update the same `C[i][j]` (or the same shared `sum`). That is the data race from Task 02: load, add, store interleave and updates are lost, giving wrong and run-to-run different results. It can be made correct with `reduction(+:sum)` on the `k` loop, but then a parallel region is started for every `(i, j)` pair, which is a lot of overhead, and the order of the additions changes, so the floating-point result can differ in the last bits (one reason the verification uses a tolerance). For the same reason `collapse` must not be extended to `k` (`collapse(3)`) without a reduction: the `k` iterations of one output element would land on different threads.

### `parallel for` on `i` alone vs `collapse(2)` on `(i, j)`: for which N would you expect `collapse(2)` to matter most?

`collapse(2)` increases the number of independent iterations from `N` to `N²`. It matters when `N` is not much larger than the thread count, or when `N` does not divide evenly by it, because then the outer loop alone leaves threads idle or unevenly loaded (for example `N = 8` to `32` on a 16-thread machine, or `N = 10` on 4 threads). For the sizes measured here (`N = 256..1024` on up to 8 threads) the outer loop already has hundreds of iterations per thread, the load is balanced, and `collapse(2)` is expected to make little or no difference. _(Observed on my machine: fill in.)_

### Why accumulate into a local `sum` instead of `C[i][j] += ...` inside the `k` loop?

A local scalar lives in a register, so the `k` loop does two loads (`A`, `B`) per step and one store to `C` at the very end. With `C[i][j] += ...` the compiler may have to load and store `C[i][j]` in memory on every `k` step, adding memory traffic, and in the parallel code neighbouring `C` elements written by different threads can share cache lines, causing false sharing.

### Bonus: loop order i-k-j instead of i-j-k

In i-j-k the innermost loop reads `B[k][j]` down a **column** of `B` (stride `N` doubles), which uses the cache poorly. In i-k-j the innermost loop runs over `j`, reading a row of `B` and updating a row of `C`, both with unit stride. The usual result is a noticeably faster serial baseline for large `N`, with the gap growing as the matrices stop fitting in cache. The effect on parallel scaling is not obvious in advance: the faster i-k-j loop is more bandwidth-limited per thread, so it may scale differently from i-j-k. Report both the serial time change and the parallel speedup change from your tables. _(Observed on my machine: fill in.)_
