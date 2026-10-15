#include <omp.h>

#include <algorithm>
#include <cstdio>
#include <set>
#include <vector>

#include "omp_bench.hpp"

using namespace std;

static long sumSerial(const long* a, long n) {
    long sum = 0;
    for (long i = 0; i < n; i++) sum += a[i];
    return sum;
}

static long sumNaiveParallelBroken(const long* a, long n) {
    long sum = 0;
#pragma omp parallel for
    for (long i = 0; i < n; i++) sum += a[i];
    return sum;
}

static long sumReduction(const long* a, long n) {
    long sum = 0;
#pragma omp parallel for reduction(+ : sum)
    for (long i = 0; i < n; i++) sum += a[i];
    return sum;
}

static long sumCritical(const long* a, long n) {
    long sum = 0;
#pragma omp parallel for
    for (long i = 0; i < n; i++) {
#pragma omp critical
        sum += a[i];
    }
    return sum;
}

static void raceDemo(const vector<long>& a, long expected, const vector<int>& threadCounts) {
    const int runs = 10;
    printf("| Threads | Runs | Wrong results | Distinct results | Smallest result | Largest result |\n");
    printf("|---:|---:|---:|---:|---:|---:|\n");
    for (int threads : threadCounts) {
        bench::setThreads(threads);
        set<long> distinct;
        int wrong = 0;
        long lo = expected, hi = expected;
        for (int r = 0; r < runs; r++) {
            long result = sumNaiveParallelBroken(a.data(), static_cast<long>(a.size()));
            distinct.insert(result);
            if (result != expected) wrong++;
            lo = min(lo, result);
            hi = max(hi, result);
        }
        printf("| %d | %d | %d | %zu | %ld | %ld |\n", threads, runs, wrong, distinct.size(), lo, hi);
    }
    printf("\n");
}

int main(int argc, char** argv) {
    bench::Options opt = bench::parseOptions(argc, argv, {10000000L, 100000000L});

    bench::printMachineInfo("Task 2 - Sum of Array Elements");

    for (long n : opt.sizes) {
        vector<long> a(n);
        for (long i = 0; i < n; i++) a[i] = i % 100;

        bench::setThreads(1);
        long expected = sumSerial(a.data(), n);
        double tSerial = bench::bestOf(opt.repeats, [&] { expected = sumSerial(a.data(), n); });

        printf("### N = %ld (serial sum = %ld, serial time = %.6f s)\n\n", n, expected, tSerial);

        printf("Naive parallel sum without reduction (data race), repeated 10 times per thread count:\n\n");
        raceDemo(a, expected, opt.threads);

        printf("| Threads | Serial (s) | Reduction (s) | Reduction speedup | Reduction correct | Critical (s) | Critical correct |\n");
        printf("|---:|---:|---:|---:|:---:|---:|:---:|\n");

        for (int threads : opt.threads) {
            bench::setThreads(threads);

            bool reductionOk = true;
            sumReduction(a.data(), n);
            double tReduction = bench::bestOf(opt.repeats, [&] {
                if (sumReduction(a.data(), n) != expected) reductionOk = false;
            });

            char criticalTime[32] = "skipped";
            const char* criticalOk = "-";
            if (n <= opt.criticalMax) {
                long result = 0;
                double tCritical = bench::bestOf(1, [&] { result = sumCritical(a.data(), n); });
                snprintf(criticalTime, sizeof(criticalTime), "%.6f", tCritical);
                criticalOk = (result == expected) ? "yes" : "NO";
            }

            printf("| %d | %.6f | %.6f | %.2f | %s | %s | %s |\n", threads, tSerial, tReduction,
                        tSerial / tReduction, reductionOk ? "yes" : "NO", criticalTime, criticalOk);
            if (!reductionOk) {
                fprintf(stderr, "Reduction result mismatch for N=%ld, threads=%d\n", n, threads);
                return 1;
            }
        }
        printf("\n");
    }
    return 0;
}
