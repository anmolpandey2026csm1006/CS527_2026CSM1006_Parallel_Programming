#include <omp.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <vector>

#include "omp_bench.hpp"

using namespace std;

static void matmulSerial(const double* A, const double* B, double* C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0.0;
            for (int k = 0; k < n; k++) sum += A[(long)i * n + k] * B[(long)k * n + j];
            C[(long)i * n + j] = sum;
        }
    }
}

static void matmulOuter(const double* A, const double* B, double* C, int n) {
#pragma omp parallel for
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0.0;
            for (int k = 0; k < n; k++) sum += A[(long)i * n + k] * B[(long)k * n + j];
            C[(long)i * n + j] = sum;
        }
    }
}

static void matmulCollapse(const double* A, const double* B, double* C, int n) {
#pragma omp parallel for collapse(2)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0.0;
            for (int k = 0; k < n; k++) sum += A[(long)i * n + k] * B[(long)k * n + j];
            C[(long)i * n + j] = sum;
        }
    }
}

static void matmulSerialIKJ(const double* A, const double* B, double* C, int n) {
    fill(C, C + (long)n * n, 0.0);
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < n; k++) {
            double aik = A[(long)i * n + k];
            for (int j = 0; j < n; j++) C[(long)i * n + j] += aik * B[(long)k * n + j];
        }
    }
}

static void matmulOuterIKJ(const double* A, const double* B, double* C, int n) {
    fill(C, C + (long)n * n, 0.0);
#pragma omp parallel for
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < n; k++) {
            double aik = A[(long)i * n + k];
            for (int j = 0; j < n; j++) C[(long)i * n + j] += aik * B[(long)k * n + j];
        }
    }
}

static void matmulParallelKBroken(const double* A, const double* B, double* C, int n) {
    fill(C, C + (long)n * n, 0.0);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
#pragma omp parallel for
            for (int k = 0; k < n; k++) C[(long)i * n + j] += A[(long)i * n + k] * B[(long)k * n + j];
        }
    }
}

static bool closeEnough(const vector<double>& x, const vector<double>& y, double tol = 1e-9) {
    for (size_t i = 0; i < x.size(); i++) {
        double scale = max(1.0, fabs(x[i]));
        if (fabs(x[i] - y[i]) > tol * scale) return false;
    }
    return true;
}

static void raceDemo() {
    const int n = 64;
    vector<double> A((long)n * n), B((long)n * n), ref((long)n * n), broken((long)n * n);
    for (long k = 0; k < (long)n * n; k++) {
        A[k] = static_cast<double>(k % 7);
        B[k] = static_cast<double>(k % 5);
    }
    bench::setThreads(max(2, omp_get_num_procs()));
    matmulSerial(A.data(), B.data(), ref.data(), n);
    matmulParallelKBroken(A.data(), B.data(), broken.data(), n);
    long wrong = 0;
    for (size_t i = 0; i < ref.size(); i++) {
        if (ref[i] != broken[i]) wrong++;
    }
    printf("### Race demonstration: plain parallel for on the k loop (no reduction), N = %d, threads = %d\n\n", n,
                omp_get_max_threads());
    printf("- Elements of C differing from the serial result: %ld of %d\n", wrong, n * n);
    if (wrong == 0) {
        printf("- 0 mismatches in this run does not make the k-loop version correct: the race only corrupts results when threads truly overlap, so re-run on a multi-core machine\n");
    }
    printf("\n");
}

static void timeVariant(const char* title, const bench::Options& opt, int n, const vector<double>& A,
                        const vector<double>& B, const vector<double>& ref,
                        void (*serialFn)(const double*, const double*, double*, int),
                        void (*outerFn)(const double*, const double*, double*, int),
                        void (*collapseFn)(const double*, const double*, double*, int), bool& allOk) {
    vector<double> C((long)n * n, 0.0);

    bench::setThreads(1);
    serialFn(A.data(), B.data(), C.data(), n);
    bool serialOk = closeEnough(ref, C);
    double tSerial = bench::bestOf(opt.repeats, [&] { serialFn(A.data(), B.data(), C.data(), n); });
    if (!serialOk) allOk = false;

    printf("#### %s, N = %d (serial: %.6f s)\n\n", title, n, tSerial);
    if (collapseFn) {
        printf("| Threads | Outer i (s) | Outer speedup | Outer efficiency | collapse(2) (s) | collapse speedup | collapse efficiency | Correct |\n");
        printf("|---:|---:|---:|---:|---:|---:|---:|:---:|\n");
    } else {
        printf("| Threads | Outer i (s) | Speedup | Efficiency | Correct |\n");
        printf("|---:|---:|---:|---:|:---:|\n");
    }

    for (int threads : opt.threads) {
        bench::setThreads(threads);

        fill(C.begin(), C.end(), 0.0);
        outerFn(A.data(), B.data(), C.data(), n);
        bool ok = closeEnough(ref, C);
        double tOuter = bench::bestOf(opt.repeats, [&] { outerFn(A.data(), B.data(), C.data(), n); });

        if (collapseFn) {
            fill(C.begin(), C.end(), 0.0);
            collapseFn(A.data(), B.data(), C.data(), n);
            ok = ok && closeEnough(ref, C);
            double tCollapse = bench::bestOf(opt.repeats, [&] { collapseFn(A.data(), B.data(), C.data(), n); });
            printf("| %d | %.6f | %.2f | %.2f | %.6f | %.2f | %.2f | %s |\n", threads, tOuter,
                        tSerial / tOuter, tSerial / tOuter / threads, tCollapse, tSerial / tCollapse,
                        tSerial / tCollapse / threads, ok ? "yes" : "NO");
        } else {
            printf("| %d | %.6f | %.2f | %.2f | %s |\n", threads, tOuter, tSerial / tOuter,
                        tSerial / tOuter / threads, ok ? "yes" : "NO");
        }
        if (!ok) allOk = false;
    }
    printf("\n");
}

int main(int argc, char** argv) {
    bool bonus = true;
    bool demo = true;
    vector<char*> forwarded;
    forwarded.push_back(argv[0]);
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--no-bonus") == 0) {
            bonus = false;
        } else if (strcmp(argv[i], "--no-race-demo") == 0) {
            demo = false;
        } else {
            forwarded.push_back(argv[i]);
        }
    }
    bench::Options opt =
        bench::parseOptions(static_cast<int>(forwarded.size()), forwarded.data(), {256L, 512L, 1024L});

    bench::printMachineInfo("Task 5 - Matrix-Matrix Multiplication (naive)");
    printf("- Matrices stored as flattened 1D row-major arrays (index i*N + j)\n");
    printf("- Correctness: parallel result compared with the serial i-j-k result, relative tolerance 1e-9\n\n");

    if (demo) raceDemo();

    bool allOk = true;
    for (long nl : opt.sizes) {
        int n = static_cast<int>(nl);
        vector<double> A((long)n * n), B((long)n * n), ref((long)n * n, 0.0);
        for (long k = 0; k < (long)n * n; k++) {
            A[k] = static_cast<double>(k % 7) * 0.5;
            B[k] = static_cast<double>(k % 5) * 0.25;
        }
        bench::setThreads(1);
        matmulSerial(A.data(), B.data(), ref.data(), n);

        printf("### N = %d\n\n", n);
        timeVariant("Loop order i-j-k", opt, n, A, B, ref, matmulSerial, matmulOuter, matmulCollapse, allOk);
        if (bonus) {
            timeVariant("Bonus: loop order i-k-j (outer i parallelised)", opt, n, A, B, ref, matmulSerialIKJ,
                        matmulOuterIKJ, nullptr, allOk);
        }
    }

    if (!allOk) {
        fprintf(stderr, "Verification FAILED\n");
        return 1;
    }
    return 0;
}
