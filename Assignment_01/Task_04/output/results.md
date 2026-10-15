## Task 4 - Matrix Addition

- Logical processors reported by OpenMP: 8
- Thread count set in code with omp_set_num_threads()
- Timing: omp_get_wtime(), minimum over repeated runs

- Matrices stored as flattened 1D row-major arrays (index i*N + j)
- Times are per call; small matrices are repeated many times inside the timed region and averaged

### N = 8 (62500 calls per timed run, serial: 0.088 us/call)

| Threads | Serial (us) | Outer loop only (us) | collapse(2) (us) | Outer speedup | collapse(2) speedup | Both match |
|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 0.088 | 0.388 | 0.388 | 0.23 | 0.23 | yes |
| 2 | 0.088 | 7.677 | 7.121 | 0.01 | 0.01 | yes |
| 4 | 0.088 | 12.510 | 12.181 | 0.01 | 0.01 | yes |
| 8 | 0.088 | 31.355 | 24.657 | 0.00 | 0.00 | yes |

### N = 4096 (1 calls per timed run, serial: 35627.106 us/call)

| Threads | Serial (us) | Outer loop only (us) | collapse(2) (us) | Outer speedup | collapse(2) speedup | Both match |
|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 35627.106 | 36419.515 | 36270.284 | 0.98 | 0.98 | yes |
| 2 | 35627.106 | 34944.147 | 35275.256 | 1.02 | 1.01 | yes |
| 4 | 35627.106 | 32095.540 | 33169.608 | 1.11 | 1.07 | yes |
| 8 | 35627.106 | 40926.954 | 43076.168 | 0.87 | 0.83 | yes |

