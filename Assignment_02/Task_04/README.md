# Task 04 – Matrix Transpose

**Goal:** write an OpenMP program to compute the transpose of a matrix.

---

## Implementation

| File | Contents |
|---|---|
| `source/transpose.cpp` | `transposeSerial`, `transposeOuter`, `transposeCollapse`, `transposeTiled`, `transposeInPlace`, verification, timing driver |

| Version | Loop structure |
|---|---|
| Serial | `B[j][i] = A[i][j]`, loops `i`, `j` |
| Outer parallel | `#pragma omp parallel for` on `i` |
| `collapse(2)` | `#pragma omp parallel for collapse(2)` on `i, j` |
| Tiled | the matrix is cut into 32 x 32 tiles; `#pragma omp parallel for collapse(2)` over the tile grid; each tile is transposed with plain loops |
| In place | for every `i`, swap `A[i][j]` and `A[j][i]` for `j > i`; `schedule(dynamic, 8)` |

- **Storage:** flattened 1D row-major arrays, element `(i, j)` at index `i*N + j`.
- **Verification:** every version is compared element by element (exact equality) with the serial out-of-place result. The output buffer is refilled with a sentinel before each parallel version, so a version that forgets to write some elements cannot pass. The in-place version is run on a fresh copy of `A` and compared after one application.
- **Sizes:** `N = 1024, 2048, 4096` by default (`--sizes 8192` needs about 2.1 GB); threads `1, 2, 4, 8`.
- **Speedups:** the three out-of-place versions are compared with the serial out-of-place transpose; the in-place version is compared with a serial in-place transpose, since the two kinds of transpose do different amounts of memory traffic.

## Build and run

```bash
make            # builds bin/task4_transpose
make check      # N = 256 and 1024, threads 1,2,4
make bench      # default sizes; writes output/results.md
./bin/task4_transpose --sizes 4096 --threads 1,2,4,8 --repeats 5
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

Paste the tables from `output/results.md` (one table per N with all five versions and the four speedup columns).

_Results to be pasted after running `make bench`._

## Observed behaviour (2–3 lines)

_Fill in: whether tiling helped, whether `collapse(2)` made a difference, and how the speedup changed with thread count._

---

## Design notes

**Why there is no race.** In the out-of-place version every iteration reads one element of `A` and writes one *different* element of `B`; no element is written twice and `A` is never written, so any division of the iterations among threads is safe without synchronisation.

**Why the in-place version only swaps `j > i`.** An in-place transpose exchanges `A[i][j]` with `A[j][i]`. If both the iteration `(i, j)` and the iteration `(j, i)` performed the swap, each pair would be swapped twice (undoing itself) and two threads could touch the same pair at once. Restricting to `j > i` makes every pair belong to exactly one iteration, so threads never share a pair. The work per row `i` shrinks as `i` grows (a triangular loop), so a `static` split would give the first threads far more work; `schedule(dynamic, 8)` hands out rows in small chunks to balance the load. In-place transpose needs a square matrix.

**Why transpose is memory-bound and why tiling is included.** The operation does no arithmetic, only data movement, and one of the two matrices is always accessed with a stride of `N` elements (reading down a column or writing down a column), which uses cache lines poorly for large `N`. Tiling processes small blocks so that the rows and columns of a block stay in cache while it is transposed; it is the same cache-blocking idea used for GEMM in the CS509 repository. Report from your own table whether it helped at the sizes you ran: the benefit depends on the cache sizes of the machine and on `N`.

**`collapse(2)`.** With `N` in the thousands, the outer loop alone already has far more iterations than threads, so `collapse(2)` is not expected to change the balance; it matters mainly for small `N`.
