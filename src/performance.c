#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "hftlob.h"

int main(int argc, char *argv[]){
    long iterations = 100000;
    if (argc > 1) {
        iterations = strtol(argv[1], NULL, 10);
        if (iterations <= 0) {
            printf("Invalid iteration count.\n");
            return 1;
        }
    }

    Limit limit;
    initLimit(&limit);
    limit.limitPrice = 100.0;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (long i = 0; i < iterations; ++i) {
        Order order;
        initOrder(&order);
        order.limit = 100.0;
        order.shares = 10.0;
        pushOrder(&limit, &order);
        removeOrder(&order);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    double latency_us = (elapsed / iterations) * 1e6;
    double throughput = iterations / elapsed;
    printf("Performed %ld operations in %f seconds\n", iterations, elapsed);
    printf("Average latency per operation: %.3f microseconds\n", latency_us);
    printf("Throughput: %.3f ops/sec\n", throughput);
    return 0;
}
