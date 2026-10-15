## Task 1 - Vector Addition

- Logical processors reported by OpenMP: 8
- Thread count set in code with omp_set_num_threads()
- Timing: omp_get_wtime(), minimum over repeated runs

### N = 100000 (serial: 0.000139 s)

| Threads | Serial (s) | Parallel (s) | Speedup | Efficiency | Match |
|---:|---:|---:|---:|---:|:---:|
| 1 | 0.000139 | 0.000229 | 0.61 | 0.61 | yes |
| 2 | 0.000139 | 0.000192 | 0.72 | 0.36 | yes |
| 4 | 0.000139 | 0.000238 | 0.58 | 0.15 | yes |
| 8 | 0.000139 | 0.000185 | 0.75 | 0.09 | yes |

### N = 1000000 (serial: 0.002507 s)

| Threads | Serial (s) | Parallel (s) | Speedup | Efficiency | Match |
|---:|---:|---:|---:|---:|:---:|
| 1 | 0.002507 | 0.002229 | 1.12 | 1.12 | yes |
| 2 | 0.002507 | 0.001624 | 1.54 | 0.77 | yes |
| 4 | 0.002507 | 0.002063 | 1.22 | 0.30 | yes |
| 8 | 0.002507 | 0.002211 | 1.13 | 0.14 | yes |

### N = 10000000 (serial: 0.021189 s)

| Threads | Serial (s) | Parallel (s) | Speedup | Efficiency | Match |
|---:|---:|---:|---:|---:|:---:|
| 1 | 0.021189 | 0.025172 | 0.84 | 0.84 | yes |
| 2 | 0.021189 | 0.027124 | 0.78 | 0.39 | yes |
| 4 | 0.021189 | 0.024333 | 0.87 | 0.22 | yes |
| 8 | 0.021189 | 0.024406 | 0.87 | 0.11 | yes |

### N = 100000000 (serial: 0.220970 s)

| Threads | Serial (s) | Parallel (s) | Speedup | Efficiency | Match |
|---:|---:|---:|---:|---:|:---:|
| 1 | 0.220970 | 0.221350 | 1.00 | 1.00 | yes |
| 2 | 0.220970 | 0.212085 | 1.04 | 0.52 | yes |
| 4 | 0.220970 | 0.194378 | 1.14 | 0.28 | yes |
| 8 | 0.220970 | 0.233899 | 0.94 | 0.12 | yes |

