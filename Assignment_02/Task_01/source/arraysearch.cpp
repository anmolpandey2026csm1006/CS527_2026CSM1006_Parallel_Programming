#include <omp.h>

#include <algorithm>
#include <cstdio>
#include <random>
#include <vector>

#include "omp_bench.hpp"

using namespace std;

static const int TARGET = 2000000000;

static long searchSerial(const int* a, long n, int target) {
    for (long i = 0; i < n; i++) {
        if (a[i] == target) return i;
    }
    return -1;
}

static long searchReductionMin(const int* a, long n, int target) {
    long first = n;
#pragma omp parallel for reduction(min : first)
    for (long i = 0; i < n; i++) {
        if (a[i] == target && i < first) first = i;
    }
    return first == n ? -1 : first;
}

static long searchEarlyExit(const int* a, long n, int target) {
    const long block = 4096;
    long blocks = (n + block - 1) / block;
    long found = -1;
    int stop = 0;

#pragma omp parallel for schedule(dynamic)
    for (long b = 0; b < blocks; b++) {
        int shouldStop;
#pragma omp atomic read
        shouldStop = stop;
        if (shouldStop) continue;

        long lo = b * block;
        long hi = min(lo + block, n);
        for (long i = lo; i < hi; i++) {
            if (a[i] == target) {
#pragma omp critical(found_update)
                {
                    if (found < 0 || i < found) found = i;
                }
#pragma omp atomic write
                stop = 1;
                break;
            }
        }
    }
    return found;
}

struct Scenario {
    const char* name;
    vector<long> positions;
};

int main(int argc, char** argv) {
    bench::Options opt = bench::parseOptions(argc, argv, {1000000L, 10000000L, 100000000L});

    bench::printMachineInfo("Task 1 - Array Search");
    printf("- Searching for the value %d, which never occurs in the random background data (values 0..999999999)\n", TARGET);
    printf("- Three scenarios per N: target near the start, target in the middle (planted twice, so the first occurrence matters), target absent\n");
    printf("- Reduction version scans the whole array and returns the first occurrence; early-exit version stops once any thread finds the target and returns some occurrence\n\n");

    for (long n : opt.sizes) {
        if (n < 100) {
            fprintf(stderr, "N must be at least 100\n");
            return 1;
        }
        vector<int> data(n);
        mt19937 gen(2024);
        uniform_int_distribution<int> dist(0, 999999999);
        for (long i = 0; i < n; i++) data[i] = dist(gen);

        vector<Scenario> scenarios = {
            {"target near the start", {n / 100}},
            {"target in the middle (two copies)", {n / 2, (3 * n) / 4}},
            {"target absent", {}},
        };

        printf("### N = %ld\n\n", n);

        for (const Scenario& sc : scenarios) {
            vector<int> saved;
            for (long p : sc.positions) {
                saved.push_back(data[p]);
                data[p] = TARGET;
            }

            const int* a = data.data();
            bench::setThreads(1);
            long reference = searchSerial(a, n, TARGET);
            double tSerial = bench::bestOf(opt.repeats, [&] { reference = searchSerial(a, n, TARGET); });

            printf("#### %s (serial index = %ld, serial time = %.6f s)\n\n", sc.name, reference, tSerial);
            printf("| Threads | Serial (s) | Reduction min (s) | Early exit (s) | Reduction speedup | Early-exit speedup | Correct |\n");
            printf("|---:|---:|---:|---:|---:|---:|:---:|\n");

            for (int threads : opt.threads) {
                bench::setThreads(threads);
                bool ok = true;

                long r1 = -2;
                double tReduction = bench::bestOf(opt.repeats, [&] { r1 = searchReductionMin(a, n, TARGET); });
                if (r1 != reference) ok = false;

                long r2 = -2;
                double tEarly = bench::bestOf(opt.repeats, [&] { r2 = searchEarlyExit(a, n, TARGET); });
                if (reference < 0) {
                    if (r2 != -1) ok = false;
                } else if (r2 < reference || r2 >= n || a[r2] != TARGET) {
                    ok = false;
                }

                printf("| %d | %.6f | %.6f | %.6f | %.2f | %.2f | %s |\n", threads, tSerial, tReduction, tEarly,
                       tSerial / tReduction, tSerial / tEarly, ok ? "yes" : "NO");
                if (!ok) {
                    fprintf(stderr, "Verification FAILED (N=%ld, %s, threads=%d)\n", n, sc.name, threads);
                    return 1;
                }
            }
            printf("\n");

            for (size_t k = 0; k < sc.positions.size(); k++) data[sc.positions[k]] = saved[k];
        }
    }
    return 0;
}
