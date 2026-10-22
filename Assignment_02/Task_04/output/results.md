## Task 4 - Matrix Transpose

- Logical processors reported by OpenMP: 8
- Thread count set in code with omp_set_num_threads()
- Timing: omp_get_wtime(), minimum over repeated runs

- Matrices stored as flattened 1D row-major arrays (element (i, j) at index i*N + j)
- Out-of-place versions write B[j][i] = A[i][j]; the in-place version swaps A[i][j] and A[j][i] for j > i
- Tiled version uses 32 x 32 tiles
- Verification: exact element-wise comparison with the serial out-of-place transpose

### N = 1024 (serial out-of-place: 0.007590 s, serial in-place: 0.009605 s)

| Threads | Serial (s) | Outer i (s) | collapse(2) (s) | Tiled (s) | In-place (s) | Outer speedup | collapse speedup | Tiled speedup | In-place speedup | All correct |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 0.007590 | 0.007674 | 0.007743 | 0.005743 | 0.007534 | 0.99 | 0.98 | 1.32 | 1.27 | yes |
| 2 | 0.007590 | 0.005623 | 0.004576 | 0.003689 | 0.005109 | 1.35 | 1.66 | 2.06 | 1.88 | yes |
| 4 | 0.007590 | 0.003887 | 0.004081 | 0.002856 | 0.004522 | 1.95 | 1.86 | 2.66 | 2.12 | yes |
| 8 | 0.007590 | 0.004758 | 0.004656 | 0.003435 | 0.003614 | 1.60 | 1.63 | 2.21 | 2.66 | yes |

### N = 2048 (serial out-of-place: 0.071838 s, serial in-place: 0.046323 s)

| Threads | Serial (s) | Outer i (s) | collapse(2) (s) | Tiled (s) | In-place (s) | Outer speedup | collapse speedup | Tiled speedup | In-place speedup | All correct |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 0.071838 | 0.111374 | 0.099354 | 0.029416 | 0.051980 | 0.65 | 0.72 | 2.44 | 0.89 | yes |
| 2 | 0.071838 | 0.080994 | 0.080382 | 0.023238 | 0.037675 | 0.89 | 0.89 | 3.09 | 1.23 | yes |
| 4 | 0.071838 | 0.090574 | 0.089771 | 0.021868 | 0.023706 | 0.79 | 0.80 | 3.29 | 1.95 | yes |
| 8 | 0.071838 | 0.080302 | 0.077551 | 0.014217 | 0.016821 | 0.89 | 0.93 | 5.05 | 2.75 | yes |

### N = 4096 (serial out-of-place: 0.543192 s, serial in-place: 0.217724 s)

| Threads | Serial (s) | Outer i (s) | collapse(2) (s) | Tiled (s) | In-place (s) | Outer speedup | collapse speedup | Tiled speedup | In-place speedup | All correct |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 0.543192 | 0.513583 | 0.509950 | 0.134823 | 0.219071 | 1.06 | 1.07 | 4.03 | 0.99 | yes |
| 2 | 0.543192 | 0.445591 | 0.453793 | 0.105535 | 0.184047 | 1.22 | 1.20 | 5.15 | 1.18 | yes |
| 4 | 0.543192 | 0.363533 | 0.360656 | 0.085554 | 0.123461 | 1.49 | 1.51 | 6.35 | 1.76 | yes |
| 8 | 0.543192 | 0.370255 | 0.337906 | 0.060229 | 0.093720 | 1.47 | 1.61 | 9.02 | 2.32 | yes |

