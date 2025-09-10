#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hftlob.h"

/* forward declarations */
typedef struct {
    double latency_us;
    double throughput;
    double elapsed;
} BenchmarkResult;
BenchmarkResult run_benchmark(long iterations);
void print_benchmark(long iterations);
int fetch_binance_snapshot(const char *symbol,
                           double *bid_price,
                           double *bid_qty,
                           double *ask_price,
                           double *ask_qty);
int run_gui(void);

int main(int argc, char* argv[]){
    printf("Running main..\n");
    for(int i=1; i<argc; ++i){
        if (strcmp(argv[i], "--test") == 0){
            printf("--test flag passed, running cuTest TestSuite..\n");
            RunAllTests();
            return 0;
        } else if (strcmp(argv[i], "--benchmark") == 0 && i + 1 < argc) {
            long iters = strtol(argv[i+1], NULL, 10);
            print_benchmark(iters);
            return 0;
        } else if (strcmp(argv[i], "--binance-snapshot") == 0) {
            const char *symbol = "ETHUSDT";
            if (i + 1 < argc) symbol = argv[i+1];
            double bp=0, bq=0, ap=0, aq=0;
            if (fetch_binance_snapshot(symbol, &bp, &bq, &ap, &aq) == 0) {
                printf("Best bid: %.2f qty %.4f\n", bp, bq);
                printf("Best ask: %.2f qty %.4f\n", ap, aq);
            }
            return 0;
        } else if (strcmp(argv[i], "--gui") == 0) {
            return run_gui();
        }
    }
    printf("No valid flag provided. Use --test, --benchmark <n>, --binance-snapshot [SYMBOL], or --gui.\n");
    return 0;
}
