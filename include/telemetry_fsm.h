//
// Created by francesco on 06/11/26.
//

#ifndef PROJECT2_TELEMETRY_FSM_H
#define PROJECT2_TELEMETRY_FSM_H

#include "parser.h"
#include "session_stats.h"

#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <thread>

enum class State { IDLE, RUN };

struct TimedMessage
{
    std::string raw;
    std::chrono::system_clock::time_point received_system;
    std::chrono::steady_clock::time_point received_steady;
};

class TelemetryFSM
{
private:
    State current_state;
    SessionStats session_stats;
    int session_number = 0;

    std::queue<TimedMessage> message_queue;
    std::mutex queue_mutex;
    std::condition_variable queue_cv;
    bool receiver_finished = false;

    std::ofstream current_log_file;
    std::string current_session_timestamp;

    void handle_idle(const ParsedMessage& msg, const TimedMessage& raw_msg);
    void handle_run(const ParsedMessage& msg, const TimedMessage& raw_msg);

public:
    TelemetryFSM();
    void start();
    void start(const std::string& input_path);
};

#endif //PROJECT2_TELEMETRY_FSM_H
