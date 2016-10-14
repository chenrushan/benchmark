#include <stdio.h>
#include <atomic>
#include <boost/algorithm/string.hpp>
#include <boost/utility/string_ref.hpp>
#include <chrono>
#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <vector>

class Timing {
public:
    ~Timing() {
        auto then = std::chrono::high_resolution_clock::now();
        auto t = std::chrono::duration<double, std::micro>(then - now_).count();
        fprintf(stderr, "[%s]: %lf\n", msg_.data(), t);
        if (timing_) {
            *timing_ = t;
        }
    }
    Timing(boost::string_ref msg) : msg_(msg) {
        now_ = std::chrono::high_resolution_clock::now();
    }
    Timing(boost::string_ref msg, double &timing)
        : msg_(msg), timing_(&timing) {
        now_ = std::chrono::high_resolution_clock::now();
    }

private:
    double *timing_ = nullptr;
    boost::string_ref msg_;
    std::chrono::high_resolution_clock::time_point now_;
};

uint64_t hash(void *key, size_t len) {
    const uint64_t m = 0xc6a4a7935bd1e995;
    const int r = 47;
    uint64_t h = 0 ^ (len * m);
    const uint64_t *data = (const uint64_t *)key;
    const uint64_t *end = data + (len / 8);

    while (data != end) {
        uint64_t k = *data++;
        k *= m;
        k ^= k >> r;
        k *= m;
        h ^= k;
        h *= m;
    }

    const unsigned char *data2 = (const unsigned char *)data;
    switch (len & 7) {
        case 7:
            h ^= (uint64_t)(data2[6]) << 48;
        case 6:
            h ^= (uint64_t)(data2[5]) << 40;
        case 5:
            h ^= (uint64_t)(data2[4]) << 32;
        case 4:
            h ^= (uint64_t)(data2[3]) << 24;
        case 3:
            h ^= (uint64_t)(data2[2]) << 16;
        case 2:
            h ^= (uint64_t)(data2[1]) << 8;
        case 1:
            h ^= (uint64_t)(data2[0]);
            h *= m;
    };

    h ^= h >> r;
    h *= m;
    h ^= h >> r;
    return h;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "[USAGE]: ./cmd [num_threads] [num iterations]"
                  << std::endl;
        std::exit(1);
    }

    size_t num_threads = std::stoi(argv[1]);
    Timing t("time");
    size_t num_times = std::stoi(argv[2]);
    size_t num_times_each_thread = num_times / num_threads;
    std::vector<std::thread> threads;
    double res = 0;

    for (int t = 0; t < num_threads; ++t) {
        threads.push_back(std::thread([&]() {
            // busy calculation
            double k = 0;
            for (int i = 0; i < num_times_each_thread; ++i) {
                for (auto i = 1; i < 10000; ++i) {
                    k += hash((void *)&i, sizeof(i));
                }
            }
            res += k;
        }));
    }

    for (auto &th : threads) {
        th.join();
    }
    std::cout << res << std::endl;
    return 0;
}
