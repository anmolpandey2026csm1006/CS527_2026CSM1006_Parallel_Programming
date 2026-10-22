## Task 3 - Histogram Calculation

- Logical processors reported by OpenMP: 8
- Thread count set in code with omp_set_num_threads()
- Timing: omp_get_wtime(), minimum over repeated runs

- Input: random integers 0..9, ten bins
- Verification: every histogram is compared bin by bin with the serial histogram
- The critical version is only run for N <= --critical-max (default 10000000)

### N = 1000000 (serial time = 0.001461 s)

Serial histogram: 0:100030 1:100025 2:100446 3:99713 4:99923 5:100207 6:99808 7:99650 8:100212 9:99986 

Naive parallel histogram without any protection (data race), repeated 10 times per thread count:

| Threads | Runs | Wrong histograms | Distinct histograms | Smallest total counted (should be 1000000) |
|---:|---:|---:|---:|---:|
| 1 | 10 | 0 | 1 | 1000000 |
| 2 | 10 | 10 | 10 | 552564 |
| 4 | 10 | 10 | 10 | 268853 |
| 8 | 10 | 10 | 10 | 471307 |

| Threads | Serial (s) | Atomic (s) | Critical (s) | Reduction (s) | Private + merge (s) | Private speedup | All correct |
|---:|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 0.001461 | 0.009778 | 0.025442 | 0.000585 | 0.000579 | 2.53 | yes |
| 2 | 0.001461 | 0.020936 | 0.090815 | 0.000530 | 0.000444 | 3.29 | yes |
| 4 | 0.001461 | 0.027116 | 0.100732 | 0.000517 | 0.000509 | 2.87 | yes |
| 8 | 0.001461 | 0.033331 | 0.156570 | 0.000387 | 0.000385 | 3.80 | yes |

### N = 10000000 (serial time = 0.007241 s)

Serial histogram: 0:1000110 1:1000650 2:998836 3:999336 4:1000046 5:1000015 6:1001409 7:999542 8:1000072 9:999984 

Naive parallel histogram without any protection (data race), repeated 10 times per thread count:

| Threads | Runs | Wrong histograms | Distinct histograms | Smallest total counted (should be 10000000) |
|---:|---:|---:|---:|---:|
| 1 | 10 | 0 | 1 | 10000000 |
| 2 | 10 | 10 | 10 | 5147183 |
| 4 | 10 | 10 | 10 | 4079783 |
| 8 | 10 | 10 | 10 | 3720094 |

| Threads | Serial (s) | Atomic (s) | Critical (s) | Reduction (s) | Private + merge (s) | Private speedup | All correct |
|---:|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 0.007241 | 0.083248 | 0.253160 | 0.006837 | 0.006830 | 1.06 | yes |
| 2 | 0.007241 | 0.209603 | 0.880787 | 0.004383 | 0.004343 | 1.67 | yes |
| 4 | 0.007241 | 0.269904 | 1.040035 | 0.003456 | 0.004142 | 1.75 | yes |
| 8 | 0.007241 | 0.328898 | 1.665245 | 0.002627 | 0.002328 | 3.11 | yes |

### N = 100000000 (serial time = 0.067500 s)

Serial histogram: 0:10002383 1:9999154 2:10000540 3:10003243 4:9999373 5:9999389 6:9994096 7:10001361 8:10000937 9:9999524 

Naive parallel histogram without any protection (data race), repeated 10 times per thread count:

| Threads | Runs | Wrong histograms | Distinct histograms | Smallest total counted (should be 100000000) |
|---:|---:|---:|---:|---:|
| 1 | 10 | 0 | 1 | 100000000 |
| 2 | 10 | 10 | 10 | 51027713 |
| 4 | 10 | 10 | 10 | 36461788 |
| 8 | 10 | 10 | 10 | 33000559 |

| Threads | Serial (s) | Atomic (s) | Critical (s) | Reduction (s) | Private + merge (s) | Private speedup | All correct |
|---:|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 0.067500 | 0.827331 | skipped | 0.067176 | 0.067569 | 1.00 | yes |
| 2 | 0.067500 | 2.087510 | skipped | 0.042206 | 0.042697 | 1.58 | yes |
| 4 | 0.067500 | 2.706379 | skipped | 0.033119 | 0.033461 | 2.02 | yes |
| 8 | 0.067500 | 3.285169 | skipped | 0.023347 | 0.023021 | 2.93 | yes |

