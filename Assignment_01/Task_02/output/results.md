## Task 2 - Sum of Array Elements

- Logical processors reported by OpenMP: 8
- Thread count set in code with omp_set_num_threads()
- Timing: omp_get_wtime(), minimum over repeated runs

### N = 10000000 (serial sum = 495000000, serial time = 0.009231 s)

Naive parallel sum without reduction (data race), repeated 10 times per thread count:

| Threads | Runs | Wrong results | Distinct results | Smallest result | Largest result |
|---:|---:|---:|---:|---:|---:|
| 1 | 10 | 0 | 1 | 495000000 | 495000000 |
| 2 | 10 | 10 | 1 | 247500000 | 495000000 |
| 4 | 10 | 10 | 1 | 123750000 | 495000000 |
| 8 | 10 | 10 | 2 | 61875000 | 495000000 |

| Threads | Serial (s) | Reduction (s) | Reduction speedup | Reduction correct | Critical (s) | Critical correct |
|---:|---:|---:|---:|:---:|---:|:---:|
| 1 | 0.009231 | 0.007474 | 1.24 | yes | 0.253362 | yes |
| 2 | 0.009231 | 0.006584 | 1.40 | yes | 0.830250 | yes |
| 4 | 0.009231 | 0.004970 | 1.86 | yes | 1.036202 | yes |
| 8 | 0.009231 | 0.004282 | 2.16 | yes | 1.475945 | yes |

### N = 100000000 (serial sum = 4950000000, serial time = 0.081549 s)

Naive parallel sum without reduction (data race), repeated 10 times per thread count:

| Threads | Runs | Wrong results | Distinct results | Smallest result | Largest result |
|---:|---:|---:|---:|---:|---:|
| 1 | 10 | 0 | 1 | 4950000000 | 4950000000 |
| 2 | 10 | 10 | 1 | 2475000000 | 4950000000 |
| 4 | 10 | 10 | 1 | 1237500000 | 4950000000 |
| 8 | 10 | 10 | 1 | 618750000 | 4950000000 |

| Threads | Serial (s) | Reduction (s) | Reduction speedup | Reduction correct | Critical (s) | Critical correct |
|---:|---:|---:|---:|:---:|---:|:---:|
| 1 | 0.081549 | 0.082549 | 0.99 | yes | skipped | - |
| 2 | 0.081549 | 0.046368 | 1.76 | yes | skipped | - |
| 4 | 0.081549 | 0.047295 | 1.72 | yes | skipped | - |
| 8 | 0.081549 | 0.046801 | 1.74 | yes | skipped | - |

