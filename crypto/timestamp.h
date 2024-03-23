#pragma once
#include <chrono>
// #include <sys/time.h>

// class Timestamp
// {
// public:
//     struct timeval tv;
//     Timestamp()
//     {
//         gettimeofday(&tv, NULL);
//     }

//     int operator-(Timestamp &t2)
//     {
//         return tv.tv_usec - t2.tv.tv_usec + 1000000 * (tv.tv_sec - t2.tv.tv_sec);
//     }

//     void renew()
//     {
//         gettimeofday(&tv, NULL);
//     }
// };

class Timestamp
{
public:
    std::chrono::time_point<std::chrono::high_resolution_clock> time_point;
    Timestamp()
    {
        time_point = std::chrono::high_resolution_clock::now();
    }

    uint64_t operator-(Timestamp &t2)
    {
        auto duration =  time_point - t2.time_point;
        return duration.count();
    }

    void renew()
    {
        time_point = std::chrono::high_resolution_clock::now();
    }
};