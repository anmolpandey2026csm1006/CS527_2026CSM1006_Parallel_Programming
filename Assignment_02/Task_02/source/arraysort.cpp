#include <omp.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <limits>
#include <random>
#include <vector>

#include "omp_bench.hpp"

using namespace std;

static const long BASE_CASE = 1024;
static const long TASK_CUTOFF = 32768;

static void bubbleSortSerial(int* a, long n) {
    for (long pass = 0; pass < n - 1; pass++) {
        bool swapped = false;
        for (long i = 0; i + 1 < n - pass; i++) {
            if (a[i] > a[i + 1]) {
                swap(a[i], a[i + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

static void oddEvenSortParallel(int* a, long n) {
#pragma omp parallel
    {
        for (long phase = 0; phase < n; phase++) {
            long start = phase % 2;
#pragma omp for
            for (long i = start; i < n - 1; i += 2) {
                if (a[i] > a[i + 1]) swap(a[i], a[i + 1]);
            }
        }
    }
}

static void mergeRanges(int* a, int* tmp, long lo, long mid, long hi) {
    merge(a + lo, a + mid, a + mid, a + hi, tmp + lo);
    copy(tmp + lo, tmp + hi, a + lo);
}

static void mergeSortSerial(int* a, int* tmp, long lo, long hi) {
    if (hi - lo <= BASE_CASE) {
        sort(a + lo, a + hi);
        return;
    }
    long mid = lo + (hi - lo) / 2;
    mergeSortSerial(a, tmp, lo, mid);
    mergeSortSerial(a, tmp, mid, hi);
    mergeRanges(a, tmp, lo, mid, hi);
}

static void mergeSortTasks(int* a, int* tmp, long lo, long hi) {
    if (hi - lo <= TASK_CUTOFF) {
        mergeSortSerial(a, tmp, lo, hi);
        return;
    }
    long mid = lo + (hi - lo) / 2;
#pragma omp task
    mergeSortTasks(a, tmp, lo, mid);
    mergeSortTasks(a, tmp, mid, hi);
#pragma omp taskwait
    mergeRanges(a, tmp, lo, mid, hi);
}

static void mergeSortParallel(int* a, int* tmp, long n) {
#pragma omp parallel
#pragma omp single
    mergeSortTasks(a, tmp, 0, n);
}

template <typename F>
static double timeSortRuns(int repeats, const vector<int>& input, vector<int>& work, F sortFn) {
    double best = numeric_limits<double>::max();
    for (int r = 0; r < repeats; r++) {
        copy(input.begin(), input.end(), work.begin());
        double start = omp_get_wtime();
        sortFn(work.data());
        double elapsed = omp_get_wtime() - start;
        if (elapsed < best) best = elapsed;
    }
    return best;
}

static vector<int> makeInput(long n, unsigned seed) {
    vector<int> data(n);
    mt19937 gen(seed);
    uniform_int_distribution<int> dist(0, 1000000000);
    for (long i = 0; i < n; i++) data[i] = dist(gen);
    return data;
}

static int runBubbleSizes(const bench::Options& opt) {
    printf("### Bubble sort (O(N^2)): serial bubble sort vs parallel odd-even transposition sort\n\n");
    for (long n : opt.sizes) {
        vector<int> input = makeInput(n, 7);
        vector<int> reference = input;
        sort(reference.begin(), reference.end());
        vector<int> work(n);

        bench::setThreads(1);
        double tSerial = timeSortRuns(opt.repeats, input, work, [&](int* p) { bubbleSortSerial(p, n); });
        bool serialOk = (work == reference);

        printf("#### N = %ld (serial bubble sort: %.6f s, correct: %s)\n\n", n, tSerial, serialOk ? "yes" : "NO");
        printf("| Threads | Serial bubble (s) | Odd-even parallel (s) | Speedup vs serial bubble | Speedup vs odd-even on 1 thread | Efficiency (vs odd-even on 1 thread) | Correct |\n");
        printf("|---:|---:|---:|---:|---:|---:|:---:|\n");
        if (!serialOk) {
            fprintf(stderr, "Serial bubble sort produced a wrong result (N=%ld)\n", n);
            return 1;
        }

        bench::setThreads(1);
        double tOddEvenOne = timeSortRuns(opt.repeats, input, work, [&](int* p) { oddEvenSortParallel(p, n); });

        for (int threads : opt.threads) {
            bench::setThreads(threads);
            double tPar = timeSortRuns(opt.repeats, input, work, [&](int* p) { oddEvenSortParallel(p, n); });
            bool ok = (work == reference);
            printf("| %d | %.6f | %.6f | %.2f | %.2f | %.2f | %s |\n", threads, tSerial, tPar, tSerial / tPar,
                   tOddEvenOne / tPar, tOddEvenOne / tPar / threads, ok ? "yes" : "NO");
            if (!ok) {
                fprintf(stderr, "Odd-even sort verification FAILED (N=%ld, threads=%d)\n", n, threads);
                return 1;
            }
        }
        printf("\n");
    }
    return 0;
}

static int runMergeSizes(const vector<long>& sizes, const bench::Options& opt) {
    printf("### Merge sort (O(N log N)): serial recursive merge sort vs task-parallel merge sort\n\n");
    for (long n : sizes) {
        vector<int> input = makeInput(n, 11);
        vector<int> reference = input;
        sort(reference.begin(), reference.end());
        vector<int> work(n), tmp(n);

        bench::setThreads(1);
        double tSerial =
            timeSortRuns(opt.repeats, input, work, [&](int* p) { mergeSortSerial(p, tmp.data(), 0, n); });
        bool serialOk = (work == reference);

        printf("#### N = %ld (serial merge sort: %.6f s, correct: %s)\n\n", n, tSerial, serialOk ? "yes" : "NO");
        printf("| Threads | Serial merge (s) | Task-parallel merge (s) | Speedup | Efficiency | Correct |\n");
        printf("|---:|---:|---:|---:|---:|:---:|\n");
        if (!serialOk) {
            fprintf(stderr, "Serial merge sort produced a wrong result (N=%ld)\n", n);
            return 1;
        }

        for (int threads : opt.threads) {
            bench::setThreads(threads);
            double tPar =
                timeSortRuns(opt.repeats, input, work, [&](int* p) { mergeSortParallel(p, tmp.data(), n); });
            bool ok = (work == reference);
            printf("| %d | %.6f | %.6f | %.2f | %.2f | %s |\n", threads, tSerial, tPar, tSerial / tPar,
                   tSerial / tPar / threads, ok ? "yes" : "NO");
            if (!ok) {
                fprintf(stderr, "Parallel merge sort verification FAILED (N=%ld, threads=%d)\n", n, threads);
                return 1;
            }
        }
        printf("\n");
    }
    return 0;
}

int main(int argc, char** argv) {
    vector<long> mergeSizes = {1000000L, 10000000L};
    vector<char*> forwarded;
    forwarded.push_back(argv[0]);
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--merge-sizes") == 0 && i + 1 < argc) {
            mergeSizes = bench::parseLongList(argv[++i]);
        } else {
            forwarded.push_back(argv[i]);
        }
    }
    bench::Options opt = bench::parseOptions(static_cast<int>(forwarded.size()), forwarded.data(), {5000L, 20000L});

    bench::printMachineInfo("Task 2 - Array Sorting");
    printf("- Verification: every result is compared element by element with the output of the standard library sort on the same input\n");
    printf("- Each timed run sorts a fresh copy of the same random input (the copy is not timed)\n");
    printf("- Odd-even transposition sort and bubble sort are different algorithms, so the parallel-only speedup is the one measured against odd-even sort on 1 thread\n\n");

    if (runBubbleSizes(opt) != 0) return 1;
    if (runMergeSizes(mergeSizes, opt) != 0) return 1;
    return 0;
}
