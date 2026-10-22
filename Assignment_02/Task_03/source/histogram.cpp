#include <omp.h>

#include <algorithm>
#include <cstdio>
#include <random>
#include <set>
#include <vector>

#include "omp_bench.hpp"

using namespace std;

static const int BINS = 10;

static void histSerial(const int* a, long n, long* h) {
    for (int b = 0; b < BINS; b++) h[b] = 0;
    for (long i = 0; i < n; i++) h[a[i]]++;
}

static void histNaiveBroken(const int* a, long n, long* h) {
    for (int b = 0; b < BINS; b++) h[b] = 0;
#pragma omp parallel for
    for (long i = 0; i < n; i++) h[a[i]]++;
}

static void histAtomic(const int* a, long n, long* h) {
    for (int b = 0; b < BINS; b++) h[b] = 0;
#pragma omp parallel for
    for (long i = 0; i < n; i++) {
#pragma omp atomic
        h[a[i]]++;
    }
}

static void histCritical(const int* a, long n, long* h) {
    for (int b = 0; b < BINS; b++) h[b] = 0;
#pragma omp parallel for
    for (long i = 0; i < n; i++) {
#pragma omp critical
        h[a[i]]++;
    }
}

static void histReduction(const int* a, long n, long* h) {
    long local[BINS] = {0};
#pragma omp parallel for reduction(+ : local[:BINS])
    for (long i = 0; i < n; i++) local[a[i]]++;
    for (int b = 0; b < BINS; b++) h[b] = local[b];
}

static void histPrivate(const int* a, long n, long* h) {
    for (int b = 0; b < BINS; b++) h[b] = 0;
#pragma omp parallel
    {
        long mine[BINS] = {0};
#pragma omp for nowait
        for (long i = 0; i < n; i++) mine[a[i]]++;
#pragma omp critical(merge_histograms)
        {
            for (int b = 0; b < BINS; b++) h[b] += mine[b];
        }
    }
}

static bool sameHistogram(const long* x, const long* y) {
    for (int b = 0; b < BINS; b++) {
        if (x[b] != y[b]) return false;
    }
    return true;
}

static long totalCount(const long* h) {
    long total = 0;
    for (int b = 0; b < BINS; b++) total += h[b];
    return total;
}

static void raceDemo(const int* a, long n, const long* expected, const vector<int>& threadCounts) {
    const int runs = 10;
    printf("Naive parallel histogram without any protection (data race), repeated %d times per thread count:\n\n", runs);
    printf("| Threads | Runs | Wrong histograms | Distinct histograms | Smallest total counted (should be %ld) |\n", n);
    printf("|---:|---:|---:|---:|---:|\n");
    bool anyWrong = false;
    for (int threads : threadCounts) {
        bench::setThreads(threads);
        long h[BINS];
        int wrong = 0;
        set<vector<long>> distinct;
        long smallestTotal = totalCount(expected);
        for (int r = 0; r < runs; r++) {
            histNaiveBroken(a, n, h);
            if (!sameHistogram(h, expected)) wrong++;
            distinct.insert(vector<long>(h, h + BINS));
            smallestTotal = min(smallestTotal, totalCount(h));
        }
        printf("| %d | %d | %d | %zu | %ld |\n", threads, runs, wrong, distinct.size(), smallestTotal);
        if (wrong > 0) anyWrong = true;
    }
    printf("\n");
    if (!anyWrong) {
        printf("No wrong histogram appeared in these runs. That does not make the unprotected version correct: the race only corrupts counts when threads really overlap, so repeat on a multi-core machine.\n\n");
    }
}

int main(int argc, char** argv) {
    bench::Options opt = bench::parseOptions(argc, argv, {1000000L, 10000000L, 100000000L});

    bench::printMachineInfo("Task 3 - Histogram Calculation");
    printf("- Input: random integers 0..9, ten bins\n");
    printf("- Verification: every histogram is compared bin by bin with the serial histogram\n");
    printf("- The critical version is only run for N <= --critical-max (default 10000000)\n\n");

    for (long n : opt.sizes) {
        vector<int> data(n);
        mt19937 gen(99);
        uniform_int_distribution<int> dist(0, BINS - 1);
        for (long i = 0; i < n; i++) data[i] = dist(gen);
        const int* a = data.data();

        long reference[BINS];
        bench::setThreads(1);
        histSerial(a, n, reference);
        double tSerial = bench::bestOf(opt.repeats, [&] { histSerial(a, n, reference); });

        printf("### N = %ld (serial time = %.6f s)\n\n", n, tSerial);
        printf("Serial histogram: ");
        for (int b = 0; b < BINS; b++) printf("%d:%ld ", b, reference[b]);
        printf("\n\n");

        raceDemo(a, n, reference, opt.threads);

        printf("| Threads | Serial (s) | Atomic (s) | Critical (s) | Reduction (s) | Private + merge (s) | Private speedup | All correct |\n");
        printf("|---:|---:|---:|---:|---:|---:|---:|:---:|\n");

        for (int threads : opt.threads) {
            bench::setThreads(threads);
            bool ok = true;
            long h[BINS];

            double tAtomic = bench::bestOf(opt.repeats, [&] { histAtomic(a, n, h); });
            if (!sameHistogram(h, reference)) ok = false;

            char criticalTime[32] = "skipped";
            if (n <= opt.criticalMax) {
                double tCritical = bench::bestOf(1, [&] { histCritical(a, n, h); });
                snprintf(criticalTime, sizeof(criticalTime), "%.6f", tCritical);
                if (!sameHistogram(h, reference)) ok = false;
            }

            double tReduction = bench::bestOf(opt.repeats, [&] { histReduction(a, n, h); });
            if (!sameHistogram(h, reference)) ok = false;

            double tPrivate = bench::bestOf(opt.repeats, [&] { histPrivate(a, n, h); });
            if (!sameHistogram(h, reference)) ok = false;

            printf("| %d | %.6f | %.6f | %s | %.6f | %.6f | %.2f | %s |\n", threads, tSerial, tAtomic, criticalTime,
                   tReduction, tPrivate, tSerial / tPrivate, ok ? "yes" : "NO");
            if (!ok) {
                fprintf(stderr, "Histogram verification FAILED (N=%ld, threads=%d)\n", n, threads);
                return 1;
            }
        }
        printf("\n");
    }
    return 0;
}
