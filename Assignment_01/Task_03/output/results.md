## Task 3 - Maximum and Minimum of an Array (with index)

- Logical processors reported by OpenMP: 8
- Thread count set in code with omp_set_num_threads()
- Timing: omp_get_wtime(), minimum over repeated runs

### N = 10000000

#### Maximum, N = 10000000 (serial value = 2000000000 at index 2500000, serial time = 0.005826 s)

| Threads | Serial (s) | reduction, value only (s) | critical, value+index (s) | manual per-thread, value+index (s) | Manual speedup | All correct |
|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 0.005826 | 0.009465 | 0.267642 | 0.005391 | 1.08 | yes |
| 2 | 0.005826 | 0.005333 | 0.531808 | 0.003853 | 1.51 | yes |
| 4 | 0.005826 | 0.005443 | 0.948086 | 0.005722 | 1.02 | yes |
| 8 | 0.005826 | 0.004671 | 1.282278 | 0.003402 | 1.71 | yes |

#### Minimum, N = 10000000 (serial value = -5 at index 2000000, serial time = 0.005362 s)

| Threads | Serial (s) | reduction, value only (s) | critical, value+index (s) | manual per-thread, value+index (s) | Manual speedup | All correct |
|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 0.005362 | 0.009605 | 0.263658 | 0.005498 | 0.98 | yes |
| 2 | 0.005362 | 0.004999 | 0.544750 | 0.011656 | 0.46 | yes |
| 4 | 0.005362 | 0.006060 | 1.001365 | 0.005300 | 1.01 | yes |
| 8 | 0.005362 | 0.003507 | 1.273809 | 0.003383 | 1.59 | yes |

### N = 100000000

#### Maximum, N = 100000000 (serial value = 2000000000 at index 25000000, serial time = 0.057128 s)

| Threads | Serial (s) | reduction, value only (s) | critical, value+index (s) | manual per-thread, value+index (s) | Manual speedup | All correct |
|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 0.057128 | 0.096865 | skipped | 0.056139 | 1.02 | yes |
| 2 | 0.057128 | 0.048268 | skipped | 0.027198 | 2.10 | yes |
| 4 | 0.057128 | 0.044680 | skipped | 0.037235 | 1.53 | yes |
| 8 | 0.057128 | 0.032096 | skipped | 0.033296 | 1.72 | yes |

#### Minimum, N = 100000000 (serial value = -5 at index 20000000, serial time = 0.055046 s)

| Threads | Serial (s) | reduction, value only (s) | critical, value+index (s) | manual per-thread, value+index (s) | Manual speedup | All correct |
|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 0.055046 | 0.094405 | skipped | 0.054285 | 1.01 | yes |
| 2 | 0.055046 | 0.047618 | skipped | 0.027142 | 2.03 | yes |
| 4 | 0.055046 | 0.038067 | skipped | 0.030640 | 1.80 | yes |
| 8 | 0.055046 | 0.032128 | skipped | 0.033025 | 1.67 | yes |

