// ============================================================
// File Name : how_long_to_lock_a_mutex.cpp
// Creation Date : 2016-11-01
// Last Modified : Tue 01 Nov 2016 02:43:08 PM CST
// Created By : ChenRushan
// NOTE: 编译必须加上 -pthread，否则 lock mutex 的操作会被编译成空操作
// ============================================================

#include <boost/algorithm/string.hpp>
#include <boost/utility/string_ref.hpp>
#include <chrono>
#include <iostream>
#include <mutex>

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

int main(int argc, char *argv[]) {
    std::mutex m;
    std::array<int, 100> arr;
    {
        Timing t("time");
        std::lock_guard<std::mutex> lock(m);
        for (auto i = 0; i < arr.size(); ++i) {
            arr[i] = i;
        }
    }
    std::cout << arr[0] << std::endl;
    return 0;
}
