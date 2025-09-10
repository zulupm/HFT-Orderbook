#include "imgui.h"
#include "implot.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <SDL2/SDL.h>
#include <vector>
#include <chrono>
#include <thread>
#include <cstdio>

extern "C" {
#include "hftlob.h"
typedef struct {
    double latency_us;
    double throughput;
    double elapsed;
} BenchmarkResult;
BenchmarkResult run_benchmark(long iterations);
int fetch_binance_snapshot(const char *symbol,
                           double *bid_price,
                           double *bid_qty,
                           double *ask_price,
                           double *ask_qty);
}

extern "C" int run_gui(void) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window *window = SDL_CreateWindow("HFT Metrics", SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    std::vector<double> thr, lat, price, volume;
    bool running = true;
    auto last_fetch = std::chrono::steady_clock::now();
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                running = false;
        }
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        auto now = std::chrono::steady_clock::now();
        if (now - last_fetch > std::chrono::seconds(1)) {
            BenchmarkResult r = run_benchmark(10000);
            thr.push_back(r.throughput);
            lat.push_back(r.latency_us);
            double bp=0,bq=0,ap=0,aq=0;
            if (fetch_binance_snapshot("ETHUSDT", &bp, &bq, &ap, &aq) == 0) {
                price.push_back(bp);
                volume.push_back(bq);
            }
            last_fetch = now;
        }
        if (thr.size() > 100) {
            thr.erase(thr.begin());
            lat.erase(lat.begin());
            price.erase(price.begin());
            volume.erase(volume.begin());
        }

        ImGui::Begin("Metrics");
        if (ImPlot::BeginPlot("Throughput")) {
            ImPlot::PlotLine("ops/sec", thr.data(), thr.size());
            ImPlot::EndPlot();
        }
        if (ImPlot::BeginPlot("Latency (us)")) {
            ImPlot::PlotLine("latency", lat.data(), lat.size());
            ImPlot::EndPlot();
        }
        if (ImPlot::BeginPlot("Best Bid Price")) {
            ImPlot::PlotLine("price", price.data(), price.size());
            ImPlot::EndPlot();
        }
        if (ImPlot::BeginPlot("Best Bid Volume")) {
            ImPlot::PlotLine("volume", volume.data(), volume.size());
            ImPlot::EndPlot();
        }
        ImGui::End();

        ImGui::Render();
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
