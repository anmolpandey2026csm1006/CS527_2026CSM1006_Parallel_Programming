# Task 01 – Vector Addition

**Goal:** get comfortable with the basic `#pragma omp parallel for` construct.

Given `double` arrays `a[N]` and `b[N]`, compute `c[i] = a[i] + b[i]` for every `i`.

---

## Implementation

| File | Contents |
|---|---|
| `source/vecadd.cpp` | `vecAddSerial`, `vecAddParallel` (`#pragma omp parallel for`), element-wise verification, timing driver |

- Input values are `a[i] = i % 1000` and `b[i] = (i % 7) * 0.5`, which are exactly representable in `double`, so serial and parallel sums are bit-identical and the comparison uses **no tolerance** (`!=` on every element).
- Default sizes: `N = 10^5, 10^6, 10^7, 10^8`. Default thread counts: `1, 2, 4, 8`.
- Thread count is set in code with `omp_set_num_threads()`.

## Build and run

```bash
make            # builds bin/task1_vecadd
make check      # N = 10^5 and 10^6, threads 1,2,4
make bench      # full sizes; writes output/results.md
./bin/task1_vecadd --sizes 10000000 --threads 1,2,4,8 --repeats 5
```

`N = 10^8` needs about 3.2 GB of memory (four arrays of doubles).

---

## My machine

| Item | Value |
|---|---|
| CPU model | _fill in_ |
| Physical cores / logical processors | _fill in_ |
| Compiler | _fill in (`g++ --version`)_ |
| OS | _fill in_ |

## Results

Paste the tables from `output/results.md` here (one table per N, rows are thread counts, columns are serial time, parallel time, speedup, efficiency and the verification flag). The speedup-versus-threads table for the largest N is the last table.

_Results to be pasted after running `make bench`._

## Observed speedup (2–3 lines)

_Fill in from the largest-N table: what speedup you reached, at which thread count it stopped improving, and whether it matched the expectation below._

---

## Questions

### At what array size does parallelization start paying off? Why is it not beneficial for small N?

Every parallel loop pays a fixed cost: waking or creating the thread team, dividing the iterations, and the implicit barrier at the end. A vector add does only one addition and three memory accesses per element, so for small N the useful work takes about as long as that fixed overhead, and the parallel version is no faster (or slower) than the serial loop. Parallelisation starts paying off once the loop is long enough that the time saved on the iterations exceeds the fork/join cost. Read the crossover off your own tables: it is the smallest N where the parallel time drops below the serial time for 2 or more threads. _(Observed on my machine: fill in.)_

### Does speedup keep increasing linearly with thread count? Why or why not?

Not in general. Vector addition is **memory-bandwidth bound**: each element needs two loads and one store for a single floating-point add. Once a few threads are enough to saturate the memory bandwidth, extra threads just wait on memory, so the speedup flattens. Other limits: threads beyond the number of physical cores (hyper-threads share execution units and caches, oversubscribed threads share cores), the fixed fork/join overhead, and on multi-socket machines non-local memory accesses after first touch. _(Observed on my machine: fill in.)_
