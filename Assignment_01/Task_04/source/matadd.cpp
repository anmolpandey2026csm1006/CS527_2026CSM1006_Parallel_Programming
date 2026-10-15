#include <omp.h>

#include <algorithm>
#include <cstdio>
#include <vector>

#include "omp_bench.hpp"

using namespace std;

static void matAddSerial(const double* A, const double* B, double* C, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) C[(long)i * n + j] = A[(long)i * n + j] + B[(long)i * n + j];
}

static void matAddOuter(const double* A, const double* B, double* C, int n) {
#pragma omp parallel for
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) C[(long)i * n + j] = A[(long)i * n + j] + B[(long)i * n + j];
}

static void matAddCollapse(const double* A, const double* B, double* C, int n) {
#pragma omp parallel for collapse(2)
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) C[(long)i * n + j] = A[(long)i * n + j] + B[(long)i * n + j];
}

static bool identical(const vector<double>& x, const vector<double>& y) {
    for (size_t i = 0; i < x.size(); i++) {
        if (x[i] != y[i]) return false;
    }
    return true;
}

int main(int argc, char** argv) {
    bench::Options opt = bench::parseOptions(argc, argv, {8L, 4096L});

    bench::printMachineInfo("Task 4 - Matrix Addition");
    printf("- Matrices stored as flattened 1D row-major arrays (index i*N + j)\n");
    printf("- Times are per call; small matrices are repeated many times inside the timed region and averaged\n\n");

    for (long nl : opt.sizes) {
        int n = static_cast<int>(nl);
        long elements = static_cast<long>(n) * n;
        vector<double> A(elements), B(elements), cSerial(elements, 0.0), cOuter(elements, 0.0),
            cCollapse(elements, 0.0);
        for (long k = 0; k < elements; k++) {
            A[k] = static_cast<double>(k % 13);
            B[k] = static_cast<double>(k % 5) * 0.25;
        }

        long reps = max(1L, 4000000L / elements);

        bench::setThreads(1);
        matAddSerial(A.data(), B.data(), cSerial.data(), n);
        double tSerial = bench::bestOf(opt.repeats, [&] {
                             for (long r = 0; r < reps; r++) matAddSerial(A.data(), B.data(), cSerial.data(), n);
                         }) /
                         static_cast<double>(reps) * 1e6;

        printf("### N = %d (%ld calls per timed run, serial: %.3f us/call)\n\n", n, reps, tSerial);
        printf("| Threads | Serial (us) | Outer loop only (us) | collapse(2) (us) | Outer speedup | collapse(2) speedup | Both match |\n");
        printf("|---:|---:|---:|---:|---:|---:|:---:|\n");

        for (int threads : opt.threads) {
            bench::setThreads(threads);

            matAddOuter(A.data(), B.data(), cOuter.data(), n);
            double tOuter = bench::bestOf(opt.repeats, [&] {
                                for (long r = 0; r < reps; r++) matAddOuter(A.data(), B.data(), cOuter.data(), n);
                            }) /
                            static_cast<double>(reps) * 1e6;

            matAddCollapse(A.data(), B.data(), cCollapse.data(), n);
            double tCollapse = bench::bestOf(opt.repeats, [&] {
                                   for (long r = 0; r < reps; r++)
                                       matAddCollapse(A.data(), B.data(), cCollapse.data(), n);
                               }) /
                               static_cast<double>(reps) * 1e6;

            bool ok = identical(cSerial, cOuter) && identical(cSerial, cCollapse);
            printf("| %d | %.3f | %.3f | %.3f | %.2f | %.2f | %s |\n", threads, tSerial, tOuter, tCollapse,
                        tSerial / tOuter, tSerial / tCollapse, ok ? "yes" : "NO");
            if (!ok) {
                fprintf(stderr, "Verification FAILED for N=%d, threads=%d\n", n, threads);
                return 1;
            }
        }
        printf("\n");
    }
    return 0;
}
