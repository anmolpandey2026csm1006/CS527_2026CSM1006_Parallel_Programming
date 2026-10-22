## Task 2 - Array Sorting

- Logical processors reported by OpenMP: 8
- Thread count set in code with omp_set_num_threads()
- Timing: omp_get_wtime(), minimum over repeated runs

- Verification: every result is compared element by element with the output of the standard library sort on the same input
- Each timed run sorts a fresh copy of the same random input (the copy is not timed)
- Odd-even transposition sort and bubble sort are different algorithms, so the parallel-only speedup is the one measured against odd-even sort on 1 thread

### Bubble sort (O(N^2)): serial bubble sort vs parallel odd-even transposition sort

#### N = 5000 (serial bubble sort: 0.063718 s, correct: yes)

| Threads | Serial bubble (s) | Odd-even parallel (s) | Speedup vs serial bubble | Speedup vs odd-even on 1 thread | Efficiency (vs odd-even on 1 thread) | Correct |
|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 0.063718 | 0.015838 | 4.02 | 1.00 | 1.00 | yes |
| 2 | 0.063718 | 0.024130 | 2.64 | 0.66 | 0.33 | yes |
| 4 | 0.063718 | 0.033160 | 1.92 | 0.48 | 0.12 | yes |
| 8 | 0.063718 | 0.054822 | 1.16 | 0.29 | 0.04 | yes |

#### N = 20000 (serial bubble sort: 1.337894 s, correct: yes)

| Threads | Serial bubble (s) | Odd-even parallel (s) | Speedup vs serial bubble | Speedup vs odd-even on 1 thread | Efficiency (vs odd-even on 1 thread) | Correct |
|---:|---:|---:|---:|---:|---:|:---:|
| 1 | 1.337894 | 0.242695 | 5.51 | 0.99 | 0.99 | yes |
| 2 | 1.337894 | 0.172081 | 7.77 | 1.40 | 0.70 | yes |
| 4 | 1.337894 | 0.205642 | 6.51 | 1.17 | 0.29 | yes |
| 8 | 1.337894 | 0.213820 | 6.26 | 1.13 | 0.14 | yes |

### Merge sort (O(N log N)): serial recursive merge sort vs task-parallel merge sort

#### N = 1000000 (serial merge sort: 0.122898 s, correct: yes)

| Threads | Serial merge (s) | Task-parallel merge (s) | Speedup | Efficiency | Correct |
|---:|---:|---:|---:|---:|:---:|
| 1 | 0.122898 | 0.122146 | 1.01 | 1.01 | yes |
| 2 | 0.122898 | 0.071365 | 1.72 | 0.86 | yes |
| 4 | 0.122898 | 0.058803 | 2.09 | 0.52 | yes |
| 8 | 0.122898 | 0.052266 | 2.35 | 0.29 | yes |

#### N = 10000000 (serial merge sort: 1.476524 s, correct: yes)

| Threads | Serial merge (s) | Task-parallel merge (s) | Speedup | Efficiency | Correct |
|---:|---:|---:|---:|---:|:---:|
| 1 | 1.476524 | 1.475866 | 1.00 | 1.00 | yes |
| 2 | 1.476524 | 0.888898 | 1.66 | 0.83 | yes |
| 4 | 1.476524 | 0.751376 | 1.97 | 0.49 | yes |
| 8 | 1.476524 | 0.524189 | 2.82 | 0.35 | yes |

