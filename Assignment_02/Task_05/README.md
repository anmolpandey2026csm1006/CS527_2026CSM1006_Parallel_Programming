# Task 05 – 2D Convolution

**Goal:** apply a 3×3 filter to an image represented as a matrix and parallelise the computation with OpenMP.

---

## How the computation works

The 3×3 filter is placed over a 3×3 section of the image; every image value is multiplied by the corresponding filter value, the nine products are added, and the sum is divided by the filter's divisor (9 for the averaging filter). The filter then moves one position and repeats. Only **valid positions** are used (no padding), so an `N x N` image gives an `(N-2) x (N-2)` output.

Worked example from the lab sheet (5×5 image, averaging filter):

```
Input image            Output (3x3)
 10  20  30  40  50      70  80  90
 60  70  80  90 100     120 130 140
110 120 130 140 150     170 180 190
160 170 180 190 200
210 220 230 240 250
```

The first output value is `(10+20+30+60+70+80+110+120+130)/9 = 630/9 = 70`. The program runs this example first and checks the serial, outer-loop and `collapse(2)` versions against the expected matrix (`Example check ... PASS`).

---

## Implementation

| File | Contents |
|---|---|
| `source/convolution.cpp` | filter definitions, `convSerial`, `convOuter`, `convCollapse`, worked-example check, verification, timing driver |

| Version | Parallelisation |
|---|---|
| Serial | loops over output rows `i` and columns `j` |
| Outer rows | `#pragma omp parallel for` on `i` |
| `collapse(2)` | `#pragma omp parallel for collapse(2)` on `i, j` |

- **Filters** (`--filter`): `avg` (3×3 averaging / blur, divisor 9; the default and the one on the lab sheet), `sharpen` (centre 5, four neighbours -1, divisor 1), `edge` (Laplacian edge detection: centre 8, eight neighbours -1, divisor 1), or `all` to run the three in turn. These cover the blur, sharpening and edge-detection uses listed on the sheet.
- **Not flipped:** the filter is applied exactly as the sheet describes (multiply, add, divide). Mathematical convolution flips the filter first; for these symmetric filters the result is identical.
- **Exact verification:** pixel values (0 to 255) and filter weights are integers, so every sum is exact in `double` regardless of how the compiler orders the arithmetic, and the parallel outputs are compared with the serial output using exact equality. The output buffer is refilled with a sentinel before each parallel version is checked.
- **Storage:** flattened 1D row-major arrays; the image index is `i*N + j`, the output index is `i*(N-2) + j`.
- **Sizes:** `N = 512, 2048, 4096` by default (`--sizes 8192` needs about 1.6 GB); threads `1, 2, 4, 8`.

## Build and run

```bash
make            # builds bin/task5_convolution
make check      # N = 256 and 512, all three filters, threads 1,2,4
make bench      # default sizes, averaging filter; writes output/results.md
./bin/task5_convolution --sizes 2048,4096 --threads 1,2,4,8 --filter all
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

Paste the tables from `output/results.md` (the worked-example check, then one table per N and filter: serial, outer-rows and `collapse(2)` times, speedups, efficiency, correctness flag).

_Results to be pasted after running `make bench`._

## Observed behaviour (2–3 lines)

_Fill in: how the speedup scaled with threads and whether `collapse(2)` differed from parallelising the rows only._

---

## Design notes

**Why no synchronization is needed.** Every output value depends only on a 3×3 neighbourhood of the *input* image, which is never modified, and each output value is written by exactly one iteration. Different threads read overlapping parts of the input (neighbouring rows share image rows), but concurrent reads are safe, and no two threads write the same output element. The iterations are therefore fully independent, which makes convolution an ideal `parallel for` candidate.

**Rows vs `collapse(2)`.** For the sizes used here the output has hundreds to thousands of rows, many more than threads, so splitting the rows already balances the work and `collapse(2)` is not expected to help; it would matter mainly for very small images. Each row is also a long unit-stride sweep, which is expected to be at least as cache-friendly as the index recovery a collapsed loop has to do. _(Observed on my machine: fill in.)_

**Arithmetic intensity.** Each output value does 9 multiply-adds and one division on 9 loaded pixels, but neighbouring outputs reuse most of those pixels from cache, so the loop sits between the memory-bound extreme (vector addition) and the compute-bound extreme (matrix multiplication). The scaling you observe depends on which limit your machine reaches first.

**Borders.** Valid-only convolution simply drops the one-pixel border, which is why the output is smaller. Zero padding, replicated edges or wrap-around are alternatives that keep the size at `N x N` but are not implemented here, since the lab sheet specifies the valid-positions case.
