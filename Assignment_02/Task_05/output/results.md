## Task 5 - 2D Convolution (3x3 filter)

- Logical processors reported by OpenMP: 8
- Thread count set in code with omp_set_num_threads()
- Timing: omp_get_wtime(), minimum over repeated runs

- Image stored as a flattened 1D row-major array; output has (N-2) x (N-2) values (valid positions only, no padding)
- The filter is applied as written on the lab sheet (multiply, add, divide); it is not flipped
- Pixel values and filter weights are integers, so every sum is exact and results are compared with exact equality

### Worked example from the lab sheet (5x5 image, 3x3 averaging filter, valid positions only)

Output (3x3):

    70 80 90
    120 130 140
    170 180 190

Example check (serial, outer and collapse all equal the expected matrix; first value 630/9 = 70): PASS

### N = 512, filter: averaging (blur) (serial: 0.003478 s)

| Threads | Serial (s) | Outer rows (s) | collapse(2) (s) | Outer speedup | collapse speedup | Outer efficiency | Correct |
|---:|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 0.003478 | 0.003331 | 0.002894 | 1.04 | 1.20 | 1.04 | yes |
| 2 | 0.003478 | 0.002021 | 0.001199 | 1.72 | 2.90 | 0.86 | yes |
| 4 | 0.003478 | 0.001384 | 0.001695 | 2.51 | 2.05 | 0.63 | yes |
| 8 | 0.003478 | 0.001088 | 0.001227 | 3.20 | 2.83 | 0.40 | yes |

### N = 2048, filter: averaging (blur) (serial: 0.023680 s)

| Threads | Serial (s) | Outer rows (s) | collapse(2) (s) | Outer speedup | collapse speedup | Outer efficiency | Correct |
|---:|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 0.023680 | 0.027223 | 0.029223 | 0.87 | 0.81 | 0.87 | yes |
| 2 | 0.023680 | 0.015228 | 0.015310 | 1.56 | 1.55 | 0.78 | yes |
| 4 | 0.023680 | 0.016776 | 0.022730 | 1.41 | 1.04 | 0.35 | yes |
| 8 | 0.023680 | 0.016207 | 0.014174 | 1.46 | 1.67 | 0.18 | yes |

### N = 4096, filter: averaging (blur) (serial: 0.128470 s)

| Threads | Serial (s) | Outer rows (s) | collapse(2) (s) | Outer speedup | collapse speedup | Outer efficiency | Correct |
|---:|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 0.128470 | 0.119890 | 0.122803 | 1.07 | 1.05 | 1.07 | yes |
| 2 | 0.128470 | 0.063763 | 0.063523 | 2.01 | 2.02 | 1.01 | yes |
| 4 | 0.128470 | 0.060672 | 0.069216 | 2.12 | 1.86 | 0.53 | yes |
| 8 | 0.128470 | 0.046018 | 0.042860 | 2.79 | 3.00 | 0.35 | yes |

