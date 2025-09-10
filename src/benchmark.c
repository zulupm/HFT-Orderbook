#include <stdio.h>
#include <stdlib.h>
#include "hftlob.h"

#ifdef _WIN32
#include <windows.h>
static double now(void) {
    LARGE_INTEGER freq, counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / (double)freq.QuadPart;
}
#else
#include <time.h>
static double now(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}
#endif

typedef struct {
    double latency_us;
    double throughput;
    double elapsed;
} BenchmarkResult;

BenchmarkResult run_benchmark(long iterations) {
    BenchmarkResult result = {0};
    if (iterations <= 0) {
        printf("Invalid iteration count.\n");
        return result;
    }

    Limit limit;
    initLimit(&limit);
    limit.limitPrice = 100.0;

    double start = now();
    for (long i = 0; i < iterations; ++i) {
        Order order;
        initOrder(&order);
        order.limit = 100.0;
        order.shares = 10.0;
        pushOrder(&limit, &order);
        removeOrder(&order);
    }
    double end = now();

    result.elapsed = end - start;
    result.latency_us = (result.elapsed / iterations) * 1e6;
    result.throughput = iterations / result.elapsed;
    return result;
}

void print_benchmark(long iterations) {
    BenchmarkResult r = run_benchmark(iterations);
    if (r.elapsed > 0) {
        printf("Performed %ld operations in %f seconds\n", iterations, r.elapsed);
        printf("Average latency per operation: %.3f microseconds\n", r.latency_us);
        printf("Throughput: %.3f ops/sec\n", r.throughput);
    }
}
