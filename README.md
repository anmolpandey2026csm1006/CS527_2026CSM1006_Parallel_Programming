# CS527_2026CSM1006_Parallel_Programming

This is the repository of my CS527 course lab assignments on Parallel Programming.

All code is written in **C++** (C++17) and parallelised with **OpenMP**. Each assignment lives in its own folder, and each task inside an assignment has its own source, Makefile, output folder and README.

---

## Repository Structure

```
CS527_2026CSM1006_Parallel_Programming
│
├── Assignment_01                (OpenMP: arrays and matrices)
│   ├── common/                  (shared timing/argument helper header)
│   ├── Task_01                  (Vector addition)
│   ├── Task_02                  (Sum of array elements)
│   ├── Task_03                  (Maximum / minimum with index)
│   ├── Task_04                  (Matrix addition)
│   ├── Task_05                  (Matrix-matrix multiplication)
│   ├── common_wrapper.cpp
│   ├── Makefile
│   └── README.md
│
├── ...
│
├── .gitignore
└── README.md
```

New assignment folders (`Assignment_02`, ...) are added as the course progresses, keeping the same layout.

---

## Assignments

| Assignment | Topic | Tasks | Status |
|---|---|---|---|
| [Assignment_01](Assignment_01/README.md) | Introduction to OpenMP: arrays and matrices | Vector addition, array sum, max/min with index, matrix addition, matrix multiplication | Code complete; timing tables to be filled from my machine |

---

## Requirements

- A C++17 compiler with OpenMP support (GCC 9+ recommended; `max`/`min` reductions need OpenMP 4.0+)
- GNU Make
- Linux (developed and checked on Ubuntu with GCC 13)

Check your compiler with:

```bash
g++ --version
```

---

## Building and Running

Every assignment folder has a `Makefile` with the same targets:

```bash
cd Assignment_01
make          # build every task into <task>/bin/
make check    # quick correctness run at small sizes
make bench    # full benchmark; each task writes output/results.md
make clean
```

Each assignment also has an interactive menu, as in my earlier course repositories:

```bash
cd Assignment_01
g++ common_wrapper.cpp -o wrapper_app
./wrapper_app
```

Each assignment README documents its tasks, how the timings are measured, and the written answers to the lab questions.

---

## Conventions

- Serial and parallel versions of every function are both implemented, and the parallel result is verified against the serial one before any timing is reported.
- Timing uses `omp_get_wtime()`; the thread count is set in code with `omp_set_num_threads()`.
- Built binaries (`bin/`, `wrapper_app`) are not committed (see `.gitignore`).

---

## Technologies Used

- **Programming Language:** C++17
- **Parallel Programming Model:** OpenMP
- **Compiler:** GCC (g++) with `-fopenmp -Wall -Wextra`
- **Build System:** GNU Make
- **Platform:** Linux

---

> **Academic Integrity:** This repository is maintained solely for academic and educational purposes. Please do not copy or submit this work as your own.
