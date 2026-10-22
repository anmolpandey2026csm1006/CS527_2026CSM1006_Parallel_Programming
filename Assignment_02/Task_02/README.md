# Task 02 – Array Sorting

**Goal:** implement a parallel bubble sort, or parallelise a suitable sorting algorithm, using OpenMP, and **explain any synchronization required**.

Both options are implemented: a parallel bubble sort (odd-even transposition sort) and a task-parallel merge sort.

---

## Implementation

| File | Contents |
|---|---|
| `source/arraysort.cpp` | `bubbleSortSerial`, `oddEvenSortParallel`, `mergeSortSerial`, `mergeSortTasks` / `mergeSortParallel`, verification, timing driver |

| Version | Algorithm |
|---|---|
| Serial bubble sort | classic bubble sort with an early-exit "no swap" flag; O(N²) |
| Parallel odd-even transposition sort | N phases; even phases compare-exchange the pairs (0,1), (2,3), ..., odd phases the pairs (1,2), (3,4), ...; the pairs inside a phase are processed in parallel with `omp for`; O(N²) work |
| Serial merge sort | recursive; ranges of 1024 elements or fewer are sorted with the library sort, larger ranges are split, sorted recursively, and merged; O(N log N) |
| Task-parallel merge sort | same recursion, but the left half of every range larger than 32768 elements is sorted in an `omp task` while the current thread sorts the right half; `taskwait` before the merge |

- **Verification:** every result is compared element by element with the output of the standard library sort on the same input, which also proves the output is a permutation of the input (nothing lost or duplicated).
- **Fair timing:** each timed run sorts a fresh copy of the same random input; the copy is not timed.
- **Sizes:** bubble sort `N = 5000, 20000` (O(N²) work, so kept small); merge sort `N = 10^6, 10^7` (`--merge-sizes`). Threads `1, 2, 4, 8`.
- **Speedup columns:** serial bubble sort and odd-even sort are *different algorithms* (odd-even is data-independent and vectorises differently), so odd-even on 1 thread is not the same speed as bubble sort. The table therefore reports speedup against serial bubble sort *and* the parallel-only speedup against odd-even sort on 1 thread; use the latter to judge how well the parallelism scales.

## Build and run

```bash
make            # builds bin/task2_arraysort
make check      # bubble N = 2000, merge N = 200000, threads 1,2,4
make bench      # default sizes; writes output/results.md
./bin/task2_arraysort --sizes 10000 --merge-sizes 5000000 --threads 1,2,4,8
```

---

## My machine

| Item | Value |
|---|---|
| CPU model | _fill in_ |
| Physical cores / logical processors | _fill in_ |
| Compiler | _fill in (`g++ --version`)_ |
| OS | _fill in_ |

## Results

Paste the tables from `output/results.md`: the bubble/odd-even table for each N and the merge-sort table for each N.

_Results to be pasted after running `make bench`._

## Observed behaviour (2–3 lines)

_Fill in: how odd-even sort scaled, how merge sort scaled, and which one gave the better speedup._

---

## Synchronization required

### Odd-even transposition sort

- **No locks or atomics are needed inside a phase.** In one phase every compare-exchange touches a pair `(i, i+1)` with `i` of the same parity, and these pairs are disjoint, so no two threads ever read or write the same element at the same time.
- **A barrier is needed between phases.** Phase `p+1` compares elements that phase `p` just moved, and neighbouring pairs of one phase may have been handled by different threads. Every thread must finish phase `p` before any thread starts phase `p+1`. The implicit barrier at the end of each `omp for` provides exactly this.
- **One parallel region, not N.** The phase loop is placed *inside* a single `#pragma omp parallel` region so that threads are created once and only the barrier is paid per phase, instead of a full fork/join for each of the N phases. All threads execute the same phase loop, so they reach every `omp for` the same number of times, as OpenMP requires.
- **Consequence for speed:** N barriers are an unavoidable cost, and each phase does only about N/2 cheap comparisons. For small N the barriers dominate and the parallel sort can be slower than the serial one; speedup appears as N grows.

### Task-parallel merge sort

- **Disjoint data, so no locks.** The two halves of a range are sorted by two different tasks, but they work on disjoint index ranges of the data array and of the temporary array, so the tasks never conflict.
- **`taskwait` before the merge.** Merging reads both sorted halves, so the task that was spawned for the left half must have completed first. `#pragma omp taskwait` makes the parent wait for its child task (and only for it), and then the parent merges.
- **Task cut-off.** Below 32768 elements the code calls the serial merge sort directly, because creating a task for a tiny range costs more than sorting it.
- **Single producer.** The recursion is started from a `parallel` region by one thread (`omp single`); the other threads wait at the end of the single construct and pick up the queued tasks.
- **Limit on speedup:** the final merge (and the one just below it) runs on one thread or two, so the speedup is limited well below the thread count even though all the lower levels are parallel.
