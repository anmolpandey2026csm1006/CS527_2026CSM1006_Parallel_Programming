#include <omp.h>

#include <algorithm>
#include <climits>
#include <cstdio>
#include <random>
#include <vector>

#include "omp_bench.hpp"

using namespace std;

struct MaxPolicy {
    static const char* name() { return "Maximum"; }
    static bool better(int x, int y) { return x > y; }
};

struct MinPolicy {
    static const char* name() { return "Minimum"; }
    static bool better(int x, int y) { return x < y; }
};

struct alignas(64) LocalBest {
    int value;
    long index;
};

template <typename P>
static void serialIndex(const int* a, long n, int* value, long* index) {
    int bestValue = a[0];
    long bestIndex = 0;
    for (long i = 1; i < n; i++) {
        if (P::better(a[i], bestValue)) {
            bestValue = a[i];
            bestIndex = i;
        }
    }
    *value = bestValue;
    *index = bestIndex;
}

template <typename P>
static void criticalIndex(const int* a, long n, int* value, long* index) {
    int bestValue = a[0];
    long bestIndex = 0;
#pragma omp parallel for
    for (long i = 1; i < n; i++) {
#pragma omp critical
        {
            if (P::better(a[i], bestValue) || (a[i] == bestValue && i < bestIndex)) {
                bestValue = a[i];
                bestIndex = i;
            }
        }
    }
    *value = bestValue;
    *index = bestIndex;
}

template <typename P>
static void manualIndex(const int* a, long n, int* value, long* index) {
    vector<LocalBest> local(static_cast<size_t>(omp_get_max_threads()));
    for (auto& entry : local) {
        entry.value = 0;
        entry.index = -1;
    }
    int teamSize = 1;

#pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int nt = omp_get_num_threads();
#pragma omp single
        teamSize = nt;

        long chunk = (n + nt - 1) / nt;
        long lo = static_cast<long>(tid) * chunk;
        long hi = min(lo + chunk, n);
        if (lo < hi) {
            int bestValue = a[lo];
            long bestIndex = lo;
            for (long i = lo + 1; i < hi; i++) {
                if (P::better(a[i], bestValue)) {
                    bestValue = a[i];
                    bestIndex = i;
                }
            }
            local[tid].value = bestValue;
            local[tid].index = bestIndex;
        }
    }

    int bestValue = 0;
    long bestIndex = -1;
    for (int t = 0; t < teamSize; t++) {
        if (local[t].index < 0) continue;
        if (bestIndex < 0 || P::better(local[t].value, bestValue)) {
            bestValue = local[t].value;
            bestIndex = local[t].index;
        }
    }
    *value = bestValue;
    *index = bestIndex;
}

static int maxValueReduction(const int* a, long n) {
    int maxVal = a[0];
#pragma omp parallel for reduction(max : maxVal)
    for (long i = 0; i < n; i++) {
        if (a[i] > maxVal) maxVal = a[i];
    }
    return maxVal;
}

static int minValueReduction(const int* a, long n) {
    int minVal = a[0];
#pragma omp parallel for reduction(min : minVal)
    for (long i = 0; i < n; i++) {
        if (a[i] < minVal) minVal = a[i];
    }
    return minVal;
}

template <typename P>
static int valueReduction(const int* a, long n);

template <>
int valueReduction<MaxPolicy>(const int* a, long n) {
    return maxValueReduction(a, n);
}

template <>
int valueReduction<MinPolicy>(const int* a, long n) {
    return minValueReduction(a, n);
}

template <typename P>
static int runCase(const vector<int>& data, const bench::Options& opt) {
    long n = static_cast<long>(data.size());
    const int* a = data.data();

    int refValue = 0;
    long refIndex = 0;
    bench::setThreads(1);
    serialIndex<P>(a, n, &refValue, &refIndex);
    double tSerial = bench::bestOf(opt.repeats, [&] { serialIndex<P>(a, n, &refValue, &refIndex); });

    printf("#### %s, N = %ld (serial value = %d at index %ld, serial time = %.6f s)\n\n", P::name(), n,
                refValue, refIndex, tSerial);
    printf("| Threads | Serial (s) | reduction, value only (s) | critical, value+index (s) | manual per-thread, value+index (s) | Manual speedup | All correct |\n");
    printf("|---:|---:|---:|---:|---:|---:|:---:|\n");

    for (int threads : opt.threads) {
        bench::setThreads(threads);
        bool ok = true;

        int reducedValue = 0;
        double tReduction = bench::bestOf(opt.repeats, [&] { reducedValue = valueReduction<P>(a, n); });
        if (reducedValue != refValue) ok = false;

        char criticalTime[32] = "skipped";
        if (n <= opt.criticalMax) {
            int cv = 0;
            long ci = -1;
            double tCritical = bench::bestOf(1, [&] { criticalIndex<P>(a, n, &cv, &ci); });
            snprintf(criticalTime, sizeof(criticalTime), "%.6f", tCritical);
            if (cv != refValue || ci != refIndex) ok = false;
        }

        int mv = 0;
        long mi = -1;
        double tManual = bench::bestOf(opt.repeats, [&] { manualIndex<P>(a, n, &mv, &mi); });
        if (mv != refValue || mi != refIndex) ok = false;

        printf("| %d | %.6f | %.6f | %s | %.6f | %.2f | %s |\n", threads, tSerial, tReduction, criticalTime,
                    tManual, tSerial / tManual, ok ? "yes" : "NO");
        if (!ok) {
            fprintf(stderr, "Verification FAILED (%s, N=%ld, threads=%d)\n", P::name(), n, threads);
            return 1;
        }
    }
    printf("\n");
    return 0;
}

int main(int argc, char** argv) {
    bench::Options opt = bench::parseOptions(argc, argv, {10000000L, 100000000L});

    bench::printMachineInfo("Task 3 - Maximum and Minimum of an Array (with index)");

    for (long n : opt.sizes) {
        if (n < 8) {
            fprintf(stderr, "N must be at least 8\n");
            return 1;
        }
        vector<int> data(n);
        mt19937 gen(12345);
        uniform_int_distribution<int> dist(0, 1000000000);
        for (long i = 0; i < n; i++) data[i] = dist(gen);
        data[n / 4] = 2000000000;
        data[(3 * n) / 4] = 2000000000;
        data[n / 5] = -5;
        data[(4 * n) / 5] = -5;

        printf("### N = %ld\n\n", n);
        if (runCase<MaxPolicy>(data, opt) != 0) return 1;
        if (runCase<MinPolicy>(data, opt) != 0) return 1;
    }
    return 0;
}
