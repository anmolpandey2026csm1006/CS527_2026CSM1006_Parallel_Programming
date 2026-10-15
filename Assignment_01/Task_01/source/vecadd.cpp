#include <omp.h>

#include <cstdio>
#include <vector>

#include "omp_bench.hpp"

using namespace std;

static void vecAddSerial(const double* a, const double* b, double* c, long n) {
    for (long i = 0; i < n; i++) c[i] = a[i] + b[i];
}

static void vecAddParallel(const double* a, const double* b, double* c, long n) {
#pragma omp parallel for
    for (long i = 0; i < n; i++) c[i] = a[i] + b[i];
}

static bool identical(const vector<double>& x, const vector<double>& y) {
    for (size_t i = 0; i < x.size(); i++) {
        if (x[i] != y[i]) return false;
    }
    return true;
}

int main(int argc, char** argv) {
    bench::Options opt = bench::parseOptions(argc, argv, {100000L, 1000000L, 10000000L, 100000000L});

    bench::printMachineInfo("Task 1 - Vector Addition");

    for (long n : opt.sizes) {
        vector<double> a(n), b(n), cSerial(n, 0.0), cParallel(n, 0.0);
        for (long i = 0; i < n; i++) {
            a[i] = static_cast<double>(i % 1000);
            b[i] = static_cast<double>(i % 7) * 0.5;
        }

        bench::setThreads(1);
        vecAddSerial(a.data(), b.data(), cSerial.data(), n);
        double tSerial = bench::bestOf(opt.repeats, [&] {
            vecAddSerial(a.data(), b.data(), cSerial.data(), n);
        });

        printf("### N = %ld (serial: %.6f s)\n\n", n, tSerial);
        printf("| Threads | Serial (s) | Parallel (s) | Speedup | Efficiency | Match |\n");
        printf("|---:|---:|---:|---:|---:|:---:|\n");

        for (int threads : opt.threads) {
            bench::setThreads(threads);
            vecAddParallel(a.data(), b.data(), cParallel.data(), n);
            double tParallel = bench::bestOf(opt.repeats, [&] {
                vecAddParallel(a.data(), b.data(), cParallel.data(), n);
            });
            bool ok = identical(cSerial, cParallel);
            double speedup = tSerial / tParallel;
            printf("| %d | %.6f | %.6f | %.2f | %.2f | %s |\n", threads, tSerial, tParallel, speedup,
                        speedup / threads, ok ? "yes" : "NO");
            if (!ok) {
                fprintf(stderr, "Verification FAILED for N=%ld, threads=%d\n", n, threads);
                return 1;
            }
        }
        printf("\n");
    }
    return 0;
}
