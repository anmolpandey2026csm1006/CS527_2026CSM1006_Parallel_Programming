# Assignment 02 – OpenMP: Arrays and Matrices (Lab 8)

This assignment is part of the **CS527** laboratory coursework (source: `CS527_Lab_task_OpenMP_session2.docx`, Lab 8). It continues Assignment 01 with five more OpenMP problems on arrays and matrices:

- **Task 01:** Array search (existence and index of an element)
- **Task 02:** Array sorting (parallel bubble sort, plus a task-parallel merge sort)
- **Task 03:** Histogram calculation (preventing race conditions)
- **Task 04:** Matrix transpose
- **Task 05:** 2D convolution with a 3×3 filter

For every task a serial and an OpenMP version are implemented, and every parallel result is checked against a reference (the serial result, the standard library sort, or the lab sheet's worked example) before a timing row is reported. A failed check aborts the program with exit code 1.

---

# Assignment Structure

```
Assignment_02
│
├── Task_01                     (Array search)
├── Task_02                     (Array sorting)
├── Task_03                     (Histogram)
├── Task_04                     (Matrix transpose)
├── Task_05                     (2D convolution)
│       each task contains:
│       ├── source/             (the .cpp file)
│       ├── output/             (results.md written by `make bench`)
│       ├── Makefile
│       └── README.md           (details, results table, explanations)
│
├── common_wrapper.cpp
├── Makefile
└── README.md
```

### Reused code

The timing and argument-parsing helper is **not copied**: every task includes `../../Assignment_01/common/omp_bench.hpp` (see each Makefile's `-I` flag). Keep the `Assignment_01` folder next to this one, as in the repository layout, or the build will not find the header.

---

# Build and Run

```bash
cd Assignment_02
make          # builds all five tasks (bin/ inside each task folder)
make check    # small sizes, prints tables, verifies correctness
make bench    # full default sizes; writes Task_0N/output/results.md
make clean
```

Single task:

```bash
cd Task_03
make
./bin/task3_histogram --sizes 1000000,10000000 --threads 1,2,4,8 --repeats 3
```

Menu-driven alternative:

```bash
g++ common_wrapper.cpp -o wrapper_app
./wrapper_app
```

Options shared by all programs:

| Option | Meaning | Default |
|---|---|---|
| `--sizes a,b,c` | problem sizes N | listed per task below |
| `--threads 1,2,4,8` | thread counts to test | `1,2,4,8` |
| `--repeats r` | timed repetitions; the minimum is reported | `3` |
| `--critical-max n` | largest N for the slow `critical` version (Task 03) | `10000000` |

Task-specific options: Task 02 `--merge-sizes a,b` (sizes for the O(N log N) merge sort); Task 05 `--filter avg|sharpen|edge|all` (default `avg`).

---

# Problem sizes

The lab sheet does not prescribe problem sizes, so the defaults below are my own choices. They are meant to be small enough to finish on a laptop and large enough to show the effect of parallelism. Change them with `--sizes` and state the sizes you used in the report.

| Task | Default sizes | Largest-size memory (approx.) |
|---|---|---|
| 01 Array search | N = 10^6, 10^7, 10^8 (ints) | 0.4 GB |
| 02 Sorting | bubble sort N = 5 000, 20 000; merge sort N = 10^6, 10^7 | 0.2 GB |
| 03 Histogram | N = 10^6, 10^7, 10^8 (ints) | 0.4 GB |
| 04 Transpose | N = 1024, 2048, 4096 (N x N doubles) | 0.5 GB at 4096 (4 matrices); 8192 needs about 2.1 GB |
| 05 Convolution | N = 512, 2048, 4096 (N x N doubles) | 0.4 GB at 4096 (3 matrices); 8192 needs about 1.6 GB |

---

# Methodology

- **Compiler flags:** `g++ -std=c++17 -O2 -Wall -Wextra -fopenmp` (the sheet asks for a clean `-fopenmp -Wall` build).
- **Thread count:** set **in code** with `omp_set_num_threads()` for every row (the `OMP_NUM_THREADS` variable is not used).
- **Timing:** `omp_get_wtime()`, minimum over `--repeats` runs. For tasks that overwrite an output array (Tasks 04 and 05, and the sorts), an untimed warm-up call or a fresh untimed copy of the input precedes each timed run. The serial baseline is measured once per problem size and reused for every thread count.
- **Speedup** = `T_serial / T_parallel`; **efficiency** = `speedup / threads`.
- **Verification:** exact comparison everywhere (integer data, or floating-point data built from exactly representable integers so that serial and parallel sums are identical). Output buffers are refilled with a sentinel value before each parallel variant is checked, so a variant that skips elements cannot pass by accident.
- **Data layout:** matrices are flattened 1D row-major arrays (`A[i*N + j]`).
- **Race demonstrations:** Task 03 runs the unprotected histogram 10 times per thread count. Whether corrupted counts actually show up depends on the machine, so a table with 0 wrong results does not prove the code is safe; the program prints that reminder itself.

---

# Filling in the results

The timing tables are produced by the programs and are specific to the machine they run on, so they are generated with `make bench` and pasted into each task README, not written by hand.

1. Run `make bench`.
2. Open `Task_0N/output/results.md` and paste the tables into the **Results** section of `Task_0N/README.md`.
3. Fill in the **My machine** block and the 2–3 line **Observed behaviour** note in each task README.

The explanations of the algorithms and of the synchronization used are already written in each task README.

---

# Submission checklist (from the lab sheet)

| Item | Where |
|---|---|
| Source files for all five tasks, clean with `-fopenmp -Wall` | `Task_0N/source/` |
| Correctness verification for every parallel version | built into every program (`Correct` columns, example check in Task 05, non-zero exit on failure) |
| Timing tables (thread count vs execution time) for the sizes used | `Task_0N/output/results.md`, pasted into `Task_0N/README.md` |
| Speedup / efficiency numbers where requested | `Speedup` and `Efficiency` columns |
| Explanation of the synchronization used (Task 02) and race prevention (Task 03) | `Task_02/README.md`, `Task_03/README.md` |

---

# Technologies Used

- **Programming Language:** C++17
- **Parallel Programming Model:** OpenMP
- **Compiler:** GCC (g++)
- **Build System:** GNU Make
- **Platform:** Linux
