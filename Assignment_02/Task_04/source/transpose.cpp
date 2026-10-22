#include <omp.h>

#include <algorithm>
#include <cstdio>
#include <vector>

#include "omp_bench.hpp"

using namespace std;

static const long TILE = 32;

static void transposeSerial(const double* A, double* B, long n) {
    for (long i = 0; i < n; i++)
        for (long j = 0; j < n; j++) B[j * n + i] = A[i * n + j];
}

static void transposeOuter(const double* A, double* B, long n) {
#pragma omp parallel for
    for (long i = 0; i < n; i++)
        for (long j = 0; j < n; j++) B[j * n + i] = A[i * n + j];
}

static void transposeCollapse(const double* A, double* B, long n) {
#pragma omp parallel for collapse(2)
    for (long i = 0; i < n; i++)
        for (long j = 0; j < n; j++) B[j * n + i] = A[i * n + j];
}

static void transposeTiled(const double* A, double* B, long n) {
#pragma omp parallel for collapse(2)
    for (long ii = 0; ii < n; ii += TILE) {
        for (long jj = 0; jj < n; jj += TILE) {
            long iMax = min(ii + TILE, n);
            long jMax = min(jj + TILE, n);
            for (long i = ii; i < iMax; i++)
                for (long j = jj; j < jMax; j++) B[j * n + i] = A[i * n + j];
        }
    }
}

static void transposeInPlace(double* A, long n) {
#pragma omp parallel for schedule(dynamic, 8)
    for (long i = 0; i < n; i++) {
        for (long j = i + 1; j < n; j++) swap(A[i * n + j], A[j * n + i]);
    }
}

static bool identical(const vector<double>& x, const vector<double>& y) {
    for (size_t i = 0; i < x.size(); i++) {
        if (x[i] != y[i]) return false;
    }
    return true;
}

int main(int argc, char** argv) {
    bench::Options opt = bench::parseOptions(argc, argv, {1024L, 2048L, 4096L});

    bench::printMachineInfo("Task 4 - Matrix Transpose");
    printf("- Matrices stored as flattened 1D row-major arrays (element (i, j) at index i*N + j)\n");
    printf("- Out-of-place versions write B[j][i] = A[i][j]; the in-place version swaps A[i][j] and A[j][i] for j > i\n");
    printf("- Tiled version uses %ld x %ld tiles\n", TILE, TILE);
    printf("- Verification: exact element-wise comparison with the serial out-of-place transpose\n\n");

    for (long n : opt.sizes) {
        long elements = n * n;
        vector<double> A(elements), cSerial(elements, 0.0), cPar(elements, 0.0), work(elements);
        for (long k = 0; k < elements; k++) A[k] = static_cast<double>(k % 1009) + 0.5;

        bench::setThreads(1);
        transposeSerial(A.data(), cSerial.data(), n);
        double tSerial = bench::bestOf(opt.repeats, [&] { transposeSerial(A.data(), cSerial.data(), n); });

        work = A;
        transposeInPlace(work.data(), n);
        bool inPlaceBaseOk = identical(cSerial, work);
        double tSerialInPlace = bench::bestOf(opt.repeats, [&] {
            for (long i = 0; i < n; i++)
                for (long j = i + 1; j < n; j++) swap(work[i * n + j], work[j * n + i]);
        });

        printf("### N = %ld (serial out-of-place: %.6f s, serial in-place: %.6f s)\n\n", n, tSerial, tSerialInPlace);
        printf("| Threads | Serial (s) | Outer i (s) | collapse(2) (s) | Tiled (s) | In-place (s) | Outer speedup | collapse speedup | Tiled speedup | In-place speedup | All correct |\n");
        printf("|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|:---:|\n");
        if (!inPlaceBaseOk) {
            fprintf(stderr, "In-place transpose gave a wrong result (N=%ld)\n", n);
            return 1;
        }

        for (int threads : opt.threads) {
            bench::setThreads(threads);

            fill(cPar.begin(), cPar.end(), -1.0);
            transposeOuter(A.data(), cPar.data(), n);
            double tOuter = bench::bestOf(opt.repeats, [&] { transposeOuter(A.data(), cPar.data(), n); });
            bool okOuter = identical(cSerial, cPar);

            fill(cPar.begin(), cPar.end(), -1.0);
            transposeCollapse(A.data(), cPar.data(), n);
            double tCollapse = bench::bestOf(opt.repeats, [&] { transposeCollapse(A.data(), cPar.data(), n); });
            bool okCollapse = identical(cSerial, cPar);

            fill(cPar.begin(), cPar.end(), -1.0);
            transposeTiled(A.data(), cPar.data(), n);
            double tTiled = bench::bestOf(opt.repeats, [&] { transposeTiled(A.data(), cPar.data(), n); });
            bool okTiled = identical(cSerial, cPar);

            work = A;
            transposeInPlace(work.data(), n);
            bool inPlaceOk = identical(cSerial, work);
            double tInPlace = bench::bestOf(opt.repeats, [&] { transposeInPlace(work.data(), n); });

            bool ok = okOuter && okCollapse && okTiled && inPlaceOk;
            printf("| %d | %.6f | %.6f | %.6f | %.6f | %.6f | %.2f | %.2f | %.2f | %.2f | %s |\n", threads, tSerial,
                   tOuter, tCollapse, tTiled, tInPlace, tSerial / tOuter, tSerial / tCollapse, tSerial / tTiled,
                   tSerialInPlace / tInPlace, ok ? "yes" : "NO");
            if (!ok) {
                fprintf(stderr, "Transpose verification FAILED (N=%ld, threads=%d)\n", n, threads);
                return 1;
            }
        }
        printf("\n");
    }
    return 0;
}
