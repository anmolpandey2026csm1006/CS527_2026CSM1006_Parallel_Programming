#include <omp.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "omp_bench.hpp"

using namespace std;

struct Filter {
    const char* name;
    double w[9];
    double divisor;
};

static const Filter AVERAGE = {"averaging (blur)", {1, 1, 1, 1, 1, 1, 1, 1, 1}, 9.0};
static const Filter SHARPEN = {"sharpen", {0, -1, 0, -1, 5, -1, 0, -1, 0}, 1.0};
static const Filter EDGE = {"edge detection (Laplacian)", {-1, -1, -1, -1, 8, -1, -1, -1, -1}, 1.0};

static inline double applyAt(const double* img, long n, long i, long j, const Filter& f) {
    double sum = 0.0;
    for (int di = 0; di < 3; di++)
        for (int dj = 0; dj < 3; dj++) sum += f.w[di * 3 + dj] * img[(i + di) * n + (j + dj)];
    return sum / f.divisor;
}

static void convSerial(const double* img, double* out, long n, const Filter& f) {
    long m = n - 2;
    for (long i = 0; i < m; i++)
        for (long j = 0; j < m; j++) out[i * m + j] = applyAt(img, n, i, j, f);
}

static void convOuter(const double* img, double* out, long n, const Filter& f) {
    long m = n - 2;
#pragma omp parallel for
    for (long i = 0; i < m; i++)
        for (long j = 0; j < m; j++) out[i * m + j] = applyAt(img, n, i, j, f);
}

static void convCollapse(const double* img, double* out, long n, const Filter& f) {
    long m = n - 2;
#pragma omp parallel for collapse(2)
    for (long i = 0; i < m; i++)
        for (long j = 0; j < m; j++) out[i * m + j] = applyAt(img, n, i, j, f);
}

static bool identical(const vector<double>& x, const vector<double>& y) {
    for (size_t i = 0; i < x.size(); i++) {
        if (x[i] != y[i]) return false;
    }
    return true;
}

static bool exampleCheck() {
    const long n = 5;
    vector<double> img = {10,  20,  30,  40,  50,  60,  70,  80,  90,  100, 110, 120, 130,
                          140, 150, 160, 170, 180, 190, 200, 210, 220, 230, 240, 250};
    vector<double> expected = {70, 80, 90, 120, 130, 140, 170, 180, 190};
    vector<double> s(9), o(9), c(9);
    convSerial(img.data(), s.data(), n, AVERAGE);
    bench::setThreads(2);
    convOuter(img.data(), o.data(), n, AVERAGE);
    convCollapse(img.data(), c.data(), n, AVERAGE);

    printf("### Worked example from the lab sheet (5x5 image, 3x3 averaging filter, valid positions only)\n\n");
    printf("Output (3x3):\n\n");
    for (int i = 0; i < 3; i++) printf("    %.0f %.0f %.0f\n", s[i * 3], s[i * 3 + 1], s[i * 3 + 2]);
    bool ok = identical(expected, s) && identical(expected, o) && identical(expected, c);
    printf("\nExample check (serial, outer and collapse all equal the expected matrix; first value 630/9 = 70): %s\n\n",
           ok ? "PASS" : "FAIL");
    return ok;
}

int main(int argc, char** argv) {
    string filterChoice = "avg";
    vector<char*> forwarded;
    forwarded.push_back(argv[0]);
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--filter") == 0 && i + 1 < argc) {
            filterChoice = argv[++i];
        } else {
            forwarded.push_back(argv[i]);
        }
    }
    bench::Options opt =
        bench::parseOptions(static_cast<int>(forwarded.size()), forwarded.data(), {512L, 2048L, 4096L});

    vector<const Filter*> filters;
    if (filterChoice == "avg") {
        filters = {&AVERAGE};
    } else if (filterChoice == "sharpen") {
        filters = {&SHARPEN};
    } else if (filterChoice == "edge") {
        filters = {&EDGE};
    } else if (filterChoice == "all") {
        filters = {&AVERAGE, &SHARPEN, &EDGE};
    } else {
        fprintf(stderr, "Unknown --filter '%s' (use avg, sharpen, edge or all)\n", filterChoice.c_str());
        return 1;
    }

    bench::printMachineInfo("Task 5 - 2D Convolution (3x3 filter)");
    printf("- Image stored as a flattened 1D row-major array; output has (N-2) x (N-2) values (valid positions only, no padding)\n");
    printf("- The filter is applied as written on the lab sheet (multiply, add, divide); it is not flipped\n");
    printf("- Pixel values and filter weights are integers, so every sum is exact and results are compared with exact equality\n\n");

    if (!exampleCheck()) {
        fprintf(stderr, "Worked example FAILED\n");
        return 1;
    }

    for (long n : opt.sizes) {
        if (n < 3) {
            fprintf(stderr, "N must be at least 3\n");
            return 1;
        }
        long m = n - 2;
        vector<double> img(n * n);
        for (long k = 0; k < n * n; k++) img[k] = static_cast<double>((k * 31 + (k / 64) * 17) % 256);
        vector<double> cSerial(m * m, 0.0), cPar(m * m, 0.0);

        for (const Filter* f : filters) {
            bench::setThreads(1);
            convSerial(img.data(), cSerial.data(), n, *f);
            double tSerial = bench::bestOf(opt.repeats, [&] { convSerial(img.data(), cSerial.data(), n, *f); });

            printf("### N = %ld, filter: %s (serial: %.6f s)\n\n", n, f->name, tSerial);
            printf("| Threads | Serial (s) | Outer rows (s) | collapse(2) (s) | Outer speedup | collapse speedup | Outer efficiency | Correct |\n");
            printf("|---:|---:|---:|---:|---:|---:|---:|:---:|\n");

            for (int threads : opt.threads) {
                bench::setThreads(threads);

                fill(cPar.begin(), cPar.end(), -1.0);
                convOuter(img.data(), cPar.data(), n, *f);
                double tOuter = bench::bestOf(opt.repeats, [&] { convOuter(img.data(), cPar.data(), n, *f); });
                bool okOuter = identical(cSerial, cPar);

                fill(cPar.begin(), cPar.end(), -1.0);
                convCollapse(img.data(), cPar.data(), n, *f);
                double tCollapse = bench::bestOf(opt.repeats, [&] { convCollapse(img.data(), cPar.data(), n, *f); });
                bool okCollapse = identical(cSerial, cPar);

                bool ok = okOuter && okCollapse;
                printf("| %d | %.6f | %.6f | %.6f | %.2f | %.2f | %.2f | %s |\n", threads, tSerial, tOuter, tCollapse,
                       tSerial / tOuter, tSerial / tCollapse, tSerial / tOuter / threads, ok ? "yes" : "NO");
                if (!ok) {
                    fprintf(stderr, "Convolution verification FAILED (N=%ld, filter=%s, threads=%d)\n", n, f->name,
                            threads);
                    return 1;
                }
            }
            printf("\n");
        }
    }
    return 0;
}
