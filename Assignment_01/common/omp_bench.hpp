#ifndef OMP_BENCH_HPP
#define OMP_BENCH_HPP

#include <omp.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

namespace bench {

struct Options {
    vector<long> sizes;
    vector<int> threads{1, 2, 4, 8};
    int repeats = 3;
    long criticalMax = 10000000L;
};

inline vector<long> parseLongList(const string& text) {
    vector<long> values;
    stringstream ss(text);
    string item;
    while (getline(ss, item, ',')) {
        if (!item.empty()) values.push_back(stol(item));
    }
    return values;
}

inline Options parseOptions(int argc, char** argv, const vector<long>& defaultSizes) {
    Options opt;
    opt.sizes = defaultSizes;
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        bool hasValue = (i + 1 < argc);
        if (arg == "--sizes" && hasValue) {
            opt.sizes = parseLongList(argv[++i]);
        } else if (arg == "--threads" && hasValue) {
            opt.threads.clear();
            for (long t : parseLongList(argv[++i])) opt.threads.push_back(static_cast<int>(t));
        } else if (arg == "--repeats" && hasValue) {
            opt.repeats = atoi(argv[++i]);
        } else if (arg == "--critical-max" && hasValue) {
            opt.criticalMax = stol(argv[++i]);
        } else {
            fprintf(stderr,
                         "Usage: %s [--sizes a,b,c] [--threads 1,2,4,8] [--repeats r] [--critical-max n]\n",
                         argv[0]);
            exit(1);
        }
    }
    if (opt.repeats < 1) opt.repeats = 1;
    if (opt.threads.empty()) opt.threads.push_back(1);
    return opt;
}

inline double bestOf(int repeats, const function<void()>& work) {
    double best = numeric_limits<double>::max();
    for (int r = 0; r < repeats; ++r) {
        double start = omp_get_wtime();
        work();
        double elapsed = omp_get_wtime() - start;
        if (elapsed < best) best = elapsed;
    }
    return best;
}

inline void printMachineInfo(const char* taskName) {
    printf("## %s\n\n", taskName);
    printf("- Logical processors reported by OpenMP: %d\n", omp_get_num_procs());
    printf("- Thread count set in code with omp_set_num_threads()\n");
    printf("- Timing: omp_get_wtime(), minimum over repeated runs\n\n");
}

inline void setThreads(int n) { omp_set_num_threads(n); }

}  // namespace bench

#endif
