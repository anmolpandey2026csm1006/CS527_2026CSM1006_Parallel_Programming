## Task 5 - Matrix-Matrix Multiplication (naive)

- Logical processors reported by OpenMP: 8
- Thread count set in code with omp_set_num_threads()
- Timing: omp_get_wtime(), minimum over repeated runs

- Matrices stored as flattened 1D row-major arrays (index i*N + j)
- Correctness: parallel result compared with the serial i-j-k result, relative tolerance 1e-9

### Race demonstration: plain parallel for on the k loop (no reduction), N = 64, threads = 8

- Elements of C differing from the serial result: 1289 of 4096

### N = 256

#### Loop order i-j-k, N = 256 (serial: 0.016781 s)

| Threads | Outer i (s) | Outer speedup | Outer efficiency | collapse(2) (s) | collapse speedup | collapse efficiency | Correct |
|---:|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 0.016748 | 1.00 | 1.00 | 0.016717 | 1.00 | 1.00 | yes |
| 2 | 0.008674 | 1.93 | 0.97 | 0.008634 | 1.94 | 0.97 | yes |
| 4 | 0.007803 | 2.15 | 0.54 | 0.007573 | 2.22 | 0.55 | yes |
| 8 | 0.010439 | 1.61 | 0.20 | 0.009982 | 1.68 | 0.21 | yes |

#### Bonus: loop order i-k-j (outer i parallelised), N = 256 (serial: 0.010119 s)

| Threads | Outer i (s) | Speedup | Efficiency | Correct |
|---:|---:|---:|---:|:---:|
| 1 | 0.015369 | 0.66 | 0.66 | yes |
| 2 | 0.007826 | 1.29 | 0.65 | yes |
| 4 | 0.006638 | 1.52 | 0.38 | yes |
| 8 | 0.004465 | 2.27 | 0.28 | yes |

### N = 512

#### Loop order i-j-k, N = 512 (serial: 0.543573 s)

| Threads | Outer i (s) | Outer speedup | Outer efficiency | collapse(2) (s) | collapse speedup | collapse efficiency | Correct |
|---:|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 0.543788 | 1.00 | 1.00 | 0.572677 | 0.95 | 0.95 | yes |
| 2 | 0.268235 | 2.03 | 1.01 | 0.267552 | 2.03 | 1.02 | yes |
| 4 | 0.144987 | 3.75 | 0.94 | 0.142425 | 3.82 | 0.95 | yes |
| 8 | 0.120631 | 4.51 | 0.56 | 0.106788 | 5.09 | 0.64 | yes |

#### Bonus: loop order i-k-j (outer i parallelised), N = 512 (serial: 0.082966 s)

| Threads | Outer i (s) | Speedup | Efficiency | Correct |
|---:|---:|---:|---:|:---:|
| 1 | 0.097967 | 0.85 | 0.85 | yes |
| 2 | 0.054556 | 1.52 | 0.76 | yes |
| 4 | 0.058524 | 1.42 | 0.35 | yes |
| 8 | 0.040264 | 2.06 | 0.26 | yes |

### N = 1024

#### Loop order i-j-k, N = 1024 (serial: 4.978343 s)

| Threads | Outer i (s) | Outer speedup | Outer efficiency | collapse(2) (s) | collapse speedup | collapse efficiency | Correct |
|---:|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 6.563614 | 0.76 | 0.76 | 5.979237 | 0.83 | 0.83 | yes |
| 2 | 2.518843 | 1.98 | 0.99 | 2.476829 | 2.01 | 1.00 | yes |
| 4 | 1.691770 | 2.94 | 0.74 | 1.860283 | 2.68 | 0.67 | yes |
| 8 | 1.401655 | 3.55 | 0.44 | 1.539670 | 3.23 | 0.40 | yes |

#### Bonus: loop order i-k-j (outer i parallelised), N = 1024 (serial: 0.759118 s)

| Threads | Outer i (s) | Speedup | Efficiency | Correct |
|---:|---:|---:|---:|:---:|
| 1 | 0.851417 | 0.89 | 0.89 | yes |
| 2 | 0.376817 | 2.01 | 1.01 | yes |
| 4 | 0.287686 | 2.64 | 0.66 | yes |
| 8 | 0.238021 | 3.19 | 0.40 | yes |

