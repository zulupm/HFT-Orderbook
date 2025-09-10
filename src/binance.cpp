#include <curl/curl.h>
#include <iostream>
#include <string>

static size_t write_cb(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t total = size * nmemb;
    std::string *s = static_cast<std::string *>(userp);
    s->append(static_cast<char *>(contents), total);
    return total;
}

extern "C" int fetch_binance_snapshot(const char *symbol,
                                       double *bid_price,
                                       double *bid_qty,
                                       double *ask_price,
                                       double *ask_qty) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cerr << "curl_easy_init failed" << std::endl;
        return 1;
    }
    std::string url = "https://api.binance.us/api/v3/depth?limit=5&symbol=";
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

    auto parse_level = [](const std::string &src, const char *tag, double *price, double *qty) {
        std::string key = std::string("\"") + tag + "\":[[\"";
        size_t pos = src.find(key);
        if (pos == std::string::npos) return;
        pos += key.size();
        size_t end = src.find("\"", pos);
        if (end == std::string::npos) return;
        *price = std::stod(src.substr(pos, end - pos));
        size_t qty_start = src.find("\",\"", end);
        if (qty_start == std::string::npos) return;
        qty_start += 3;
        size_t qty_end = src.find("\"", qty_start);
        if (qty_end == std::string::npos) return;
        *qty = std::stod(src.substr(qty_start, qty_end - qty_start));
    };

    parse_level(response, "bids", bid_price, bid_qty);
    parse_level(response, "asks", ask_price, ask_qty);
    return 0;
}
