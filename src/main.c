#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hftlob.h"

/* forward declarations */
void run_benchmark(long iterations);
int fetch_binance_snapshot(const char *symbol);

int main(int argc, char* argv[]){
    printf("Running main..\n");
    for(int i=1; i<argc; ++i){
        if (strcmp(argv[i], "--test") == 0){
            printf("--test flag passed, running cuTest TestSuite..\n");
            RunAllTests();
            return 0;
        } else if (strcmp(argv[i], "--benchmark") == 0 && i + 1 < argc) {
            long iters = strtol(argv[i+1], NULL, 10);
            run_benchmark(iters);
            return 0;
        } else if (strcmp(argv[i], "--binance-snapshot") == 0) {
            const char *symbol = "ETHUSDT";
            if (i + 1 < argc) symbol = argv[i+1];
            fetch_binance_snapshot(symbol);
            return 0;
        }
    }
    printf("No valid flag provided. Use --test, --benchmark <n>, or --binance-snapshot [SYMBOL].\n");
    return 0;
}
