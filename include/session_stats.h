//
// Created by francesco on 07/11/26.
//

#ifndef PROJECT2_SESSION_STATS_H
#define PROJECT2_SESSION_STATS_H

#include <chrono>
#include <map>
#include <string>
#include <cstdint>
#include <fstream>

struct MessageStats
{
    uint32_t message_count;
    double cumulative_time_ms;
    std::chrono::steady_clock::time_point last_timestamp;
};

class SessionStats
{
private:
    std::map<uint16_t, MessageStats> stats_map;

public:
    void update(uint16_t id, std::chrono::steady_clock::time_point current_timestamp);
    void save_csv_and_clear(const std::string& filename);
};

#endif //PROJECT2_SESSION_STATS_H
