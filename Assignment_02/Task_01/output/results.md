## Task 1 - Array Search

- Logical processors reported by OpenMP: 8
- Thread count set in code with omp_set_num_threads()
- Timing: omp_get_wtime(), minimum over repeated runs

- Searching for the value 2000000000, which never occurs in the random background data (values 0..999999999)
- Three scenarios per N: target near the start, target in the middle (planted twice, so the first occurrence matters), target absent
- Reduction version scans the whole array and returns the first occurrence; early-exit version stops once any thread finds the target and returns some occurrence

### N = 1000000

#### target near the start (serial index = 10000, serial time = 0.000008 s)

| Threads | Serial (s) | Reduction min (s) | Early exit (s) | Reduction speedup | Early-exit speedup | Correct |
|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 0.000008 | 0.000790 | 0.000013 | 0.01 | 0.63 | yes |
| 2 | 0.000008 | 0.001274 | 0.000037 | 0.01 | 0.23 | yes |
| 4 | 0.000008 | 0.000782 | 0.000029 | 0.01 | 0.29 | yes |
| 8 | 0.000008 | 0.000887 | 0.000039 | 0.01 | 0.22 | yes |

#### target in the middle (two copies) (serial index = 500000, serial time = 0.000717 s)

| Threads | Serial (s) | Reduction min (s) | Early exit (s) | Reduction speedup | Early-exit speedup | Correct |
|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 0.000717 | 0.003796 | 0.000890 | 0.19 | 0.81 | yes |
| 2 | 0.000717 | 0.001675 | 0.000375 | 0.43 | 1.91 | yes |
| 4 | 0.000717 | 0.000896 | 0.000231 | 0.80 | 3.10 | yes |
| 8 | 0.000717 | 0.000823 | 0.000253 | 0.87 | 2.84 | yes |

#### target absent (serial index = -1, serial time = 0.001158 s)

| Threads | Serial (s) | Reduction min (s) | Early exit (s) | Reduction speedup | Early-exit speedup | Correct |
|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 0.001158 | 0.002435 | 0.000742 | 0.48 | 1.56 | yes |
| 2 | 0.001158 | 0.000541 | 0.000508 | 2.14 | 2.28 | yes |
| 4 | 0.001158 | 0.000632 | 0.000297 | 1.83 | 3.89 | yes |
| 8 | 0.001158 | 0.000506 | 0.000304 | 2.29 | 3.81 | yes |

### N = 10000000

#### target near the start (serial index = 100000, serial time = 0.000046 s)

| Threads | Serial (s) | Reduction min (s) | Early exit (s) | Reduction speedup | Early-exit speedup | Correct |
|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 0.000046 | 0.005889 | 0.000075 | 0.01 | 0.61 | yes |
| 2 | 0.000046 | 0.006443 | 0.000132 | 0.01 | 0.35 | yes |
| 4 | 0.000046 | 0.005559 | 0.000158 | 0.01 | 0.29 | yes |
| 8 | 0.000046 | 0.003697 | 0.000170 | 0.01 | 0.27 | yes |

#### target in the middle (two copies) (serial index = 5000000, serial time = 0.003173 s)

| Threads | Serial (s) | Reduction min (s) | Early exit (s) | Reduction speedup | Early-exit speedup | Correct |
|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 0.003173 | 0.005733 | 0.002768 | 0.55 | 1.15 | yes |
| 2 | 0.003173 | 0.002997 | 0.001673 | 1.06 | 1.90 | yes |
| 4 | 0.003173 | 0.005298 | 0.001520 | 0.60 | 2.09 | yes |
| 8 | 0.003173 | 0.003993 | 0.001350 | 0.79 | 2.35 | yes |

#### target absent (serial index = -1, serial time = 0.005625 s)

| Threads | Serial (s) | Reduction min (s) | Early exit (s) | Reduction speedup | Early-exit speedup | Correct |
|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 0.005625 | 0.005653 | 0.005621 | 1.00 | 1.00 | yes |
| 2 | 0.005625 | 0.011730 | 0.004816 | 0.48 | 1.17 | yes |
| 4 | 0.005625 | 0.006673 | 0.003889 | 0.84 | 1.45 | yes |
| 8 | 0.005625 | 0.004501 | 0.003300 | 1.25 | 1.70 | yes |

### N = 100000000

#### target near the start (serial index = 1000000, serial time = 0.000460 s)

| Threads | Serial (s) | Reduction min (s) | Early exit (s) | Reduction speedup | Early-exit speedup | Correct |
|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 0.000460 | 0.055946 | 0.000795 | 0.01 | 0.58 | yes |
| 2 | 0.000460 | 0.030740 | 0.001027 | 0.01 | 0.45 | yes |
| 4 | 0.000460 | 0.035286 | 0.001271 | 0.01 | 0.36 | yes |
| 8 | 0.000460 | 0.028738 | 0.001362 | 0.02 | 0.34 | yes |

#### target in the middle (two copies) (serial index = 50000000, serial time = 0.028932 s)

| Threads | Serial (s) | Reduction min (s) | Early exit (s) | Reduction speedup | Early-exit speedup | Correct |
|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 0.028932 | 0.059709 | 0.030186 | 0.48 | 0.96 | yes |
| 2 | 0.028932 | 0.029468 | 0.016007 | 0.98 | 1.81 | yes |
| 4 | 0.028932 | 0.032071 | 0.013943 | 0.90 | 2.08 | yes |
| 8 | 0.028932 | 0.029919 | 0.014022 | 0.97 | 2.06 | yes |

#### target absent (serial index = -1, serial time = 0.058917 s)

| Threads | Serial (s) | Reduction min (s) | Early exit (s) | Reduction speedup | Early-exit speedup | Correct |
|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 0.058917 | 0.056709 | 0.058629 | 1.04 | 1.00 | yes |
| 2 | 0.058917 | 0.029073 | 0.031363 | 2.03 | 1.88 | yes |
| 4 | 0.058917 | 0.029987 | 0.024028 | 1.96 | 2.45 | yes |
| 8 | 0.058917 | 0.028750 | 0.022192 | 2.05 | 2.65 | yes |

