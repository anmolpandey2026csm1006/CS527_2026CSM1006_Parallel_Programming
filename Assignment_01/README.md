# Assignment 01 – Introduction to OpenMP: Arrays and Matrices

This assignment is part of the **CS527** laboratory coursework (source: `CS527_Lab_tasks_OpenMP.docx`). It covers the basic OpenMP constructs on 1D arrays and 2D matrices:

- **Task 01:** Vector addition (`parallel for`)
- **Task 02:** Sum of array elements (data race, `reduction`, `critical`)
- **Task 03:** Maximum and minimum with index (`reduction(max/min)`, `critical`, manual per-thread reduction)
- **Task 04:** Matrix addition (outer loop only vs `collapse(2)`)
- **Task 05:** Matrix-matrix multiplication (outer loop vs `collapse(2)`, loop-order bonus)

For every task a serial and an OpenMP version are implemented, and every parallel result is checked against the serial one before a timing row is reported (a failed check aborts the program with exit code 1).

---

# Assignment Structure

```
Assignment_01
│
├── common
│   └── omp_bench.hpp           (argument parsing, best-of-N timing helper)
│
├── Task_01                     (Vector addition)
├── Task_02                     (Sum of array elements)
├── Task_03                     (Max / min with index)
├── Task_04                     (Matrix addition)
├── Task_05                     (Matrix multiplication)
│       each task contains:
│       ├── source/             (the .cpp file)
│       ├── output/             (results.md written by `make bench`)
│       ├── Makefile
│       └── README.md           (details, results table, written answers)
│
├── common_wrapper.cpp
├── Makefile
└── README.md
```

---

# Build and Run

```bash
cd Assignment_01
make          # builds all five tasks (bin/ inside each task folder)
make check    # small sizes, prints tables, verifies correctness
make bench    # full sizes from the lab sheet; writes Task_0N/output/results.md
make clean
```

Single task:

```bash
cd Task_01
make
./bin/task1_vecadd --sizes 1000000,10000000 --threads 1,2,4,8 --repeats 3
```

Menu-driven alternative:

```bash
g++ common_wrapper.cpp -o wrapper_app
./wrapper_app
```

All programs accept the same options:

| Option | Meaning | Default |
|---|---|---|
| `--sizes a,b,c` | problem sizes (N) | the sizes from the lab sheet for that task |
| `--threads 1,2,4,8` | thread counts to test | `1,2,4,8` |
| `--repeats r` | timed repetitions; the minimum is reported | `3` |
| `--critical-max n` | largest N for which the slow `critical` versions are run (Tasks 02, 03) | `10000000` |

Task 05 additionally accepts `--no-bonus` and `--no-race-demo`.

---

# Methodology

- **Compiler flags:** `g++ -std=c++17 -O2 -Wall -Wextra -fopenmp` (the sheet asks for a clean `-fopenmp -Wall` build).
- **Thread count:** set **in code** with `omp_set_num_threads()`, looping over the list given by `--threads` (the `OMP_NUM_THREADS` environment variable is not used).
- **Timing:** `omp_get_wtime()`. Each measurement is the **minimum over `--repeats` runs**, which discards cold-start effects. Where a call writes into freshly allocated output arrays (Tasks 01, 04, 05), one untimed warm-up call precedes the timed runs so first-touch page faults are not charged to the timed region. The serial time is measured once per N and reused for every thread count.
- **Speedup** = `T_serial / T_parallel`; **efficiency** = `speedup / threads`.
- **Verification:** exact comparison for the integer/exactly-representable data in Tasks 01–04; relative tolerance `1e-9` for the matrix products in Task 05.
- **Data layout:** matrices are flattened 1D row-major arrays (`C[i*N + j]`), stated explicitly in each task as the sheet requires.
- **Thread counts above the core count:** the program does not stop you from asking for more threads than the machine has cores; those rows are oversubscribed and will not speed up. The first lines of every output print how many logical processors OpenMP sees.

### Memory needed for the largest sizes

| Task | Largest N | Approximate memory |
|---|---|---|
| 01 | 10^8 | 4 arrays of doubles ≈ 3.2 GB |
| 02 | 10^8 | 1 array of longs ≈ 0.8 GB |
| 03 | 10^8 | 1 array of ints ≈ 0.4 GB |
| 04 | 4096 | 5 matrices of doubles ≈ 0.7 GB |
| 05 | 1024 | 4 matrices of doubles ≈ 32 MB |

If the machine has less free memory, drop the largest size with `--sizes` and note it in the report.

---

# Filling in the results

The timing tables are produced by the programs themselves and are specific to the machine they run on, so they are generated with `make bench` and pasted into each task README rather than being written by hand.

1. Run `make bench` (or the `bench` target of one task).
2. Open `Task_0N/output/results.md` and paste the tables into the **Results** section of `Task_0N/README.md`.
3. Fill in the **My machine** block and the 2–3 line **Observed speedup** explanation in each task README.

The written answers to the lab questions are already in each task README; they describe what is expected and why, and the "Observed" lines are where your own measurements are checked against them.

---

# Submission checklist (from the lab sheet)

| Item | Where |
|---|---|
| Source files for all five tasks, clean with `-fopenmp -Wall` | `Task_0N/source/` |
| Correctness verification for every parallel version | built into every program (`Match` / `Correct` columns, non-zero exit on failure) |
| Timing tables (threads vs time) | `Task_0N/output/results.md`, pasted into `Task_0N/README.md` |
| Speedup / parallel-efficiency numbers | `Speedup` and `Efficiency` columns |
| Written answers to every "Questions to answer" block | `Task_0N/README.md` |
| Code and observations on git | this repository |

---

# Technologies Used

- **Programming Language:** C++17
- **Parallel Programming Model:** OpenMP
- **Compiler:** GCC (g++)
- **Build System:** GNU Make
- **Platform:** Linux
