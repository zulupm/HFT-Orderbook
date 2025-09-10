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

void run_benchmark(long iterations) {
    if (iterations <= 0) {
        printf("Invalid iteration count.\n");
        return;
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

    double elapsed = end - start;
    double latency_us = (elapsed / iterations) * 1e6;
    double throughput = iterations / elapsed;
    printf("Performed %ld operations in %f seconds\n", iterations, elapsed);
    printf("Average latency per operation: %.3f microseconds\n", latency_us);
    printf("Throughput: %.3f ops/sec\n", throughput);
}
