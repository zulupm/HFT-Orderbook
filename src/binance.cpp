#include <curl/curl.h>
#include <cstring>
#include <iostream>
#include <string>

static size_t write_cb(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t total = size * nmemb;
    std::string *s = static_cast<std::string *>(userp);
    s->append(static_cast<char *>(contents), total);
    return total;
}

extern "C" int fetch_binance_snapshot(const char *symbol) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cerr << "curl_easy_init failed" << std::endl;
        return 1;
    }
    std::string url = "https://api.binance.com/api/v3/depth?limit=5&symbol=";
    url += symbol;
    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) {
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        return 1;
    }

    double best_bid_price = 0.0, best_bid_qty = 0.0;
    double best_ask_price = 0.0, best_ask_qty = 0.0;

    const char *bids = strstr(response.c_str(), "\"bids\"");
    if (bids) {
        const char *first = strchr(bids, '[');
        if (first) {
            sscanf(first, "[[\"%lf\",\"%lf\"", &best_bid_price, &best_bid_qty);
        }
    }

    const char *asks = strstr(response.c_str(), "\"asks\"");
    if (asks) {
        const char *first = strchr(asks, '[');
        if (first) {
            sscanf(first, "[[\"%lf\",\"%lf\"", &best_ask_price, &best_ask_qty);
        }
    }

    std::cout << "Best bid: " << best_bid_price << " qty " << best_bid_qty << std::endl;
    std::cout << "Best ask: " << best_ask_price << " qty " << best_ask_qty << std::endl;
    return 0;
}
