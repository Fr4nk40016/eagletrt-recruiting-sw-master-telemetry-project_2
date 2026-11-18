#include "../include/telemetry_fsm.h"
#include <filesystem>
#include <iostream>
#include <array>

#include "../include/exceptions.h"

extern "C" {
#include "../include/fake_receiver.h"
}

#include "../include/receiver_thread.h"
#include <chrono>
#include <experimental/scope>

uint64_t get_timestamp_ms()
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    return millis.count();
}

TelemetryFSM::TelemetryFSM()
{
    current_state = State::IDLE;
}

std::string resolve_input_path()
{
    const std::array<std::string, 3> candidates{
        "candump.log",
        "../candump.log",
        "../../candump.log",
    };

    for (const std::string& candidate : candidates)
    {
        std::error_code error;
        if (std::filesystem::is_regular_file(candidate, error) && !error)
        {
            return candidate;
        }
    }

    throw OpenCanError();
}

void TelemetryFSM::start()
{
    start(resolve_input_path());
}

void TelemetryFSM::start(const std::string& input_path)
{
    if (open_can(input_path.c_str()) == -1)
    {
        throw OpenCanError();
    }

    std::experimental::scope_exit can_guard([]
    {
        close_can();
    });

    std::jthread receiver(receiver_function, std::ref(message_queue), std::ref(queue_mutex), std::ref(queue_cv),
                          std::ref(receiver_finished));

    while (true)
    {
        TimedMessage timed_message;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            queue_cv.wait(lock, [this] { return !message_queue.empty() || receiver_finished; });
            if (message_queue.empty() && receiver_finished) break;

            timed_message = message_queue.front();
            message_queue.pop();
        }

        try
        {
            ParsedMessage parsed_message = Parser::parse(timed_message.raw);

            switch (current_state)
            {
            case State::IDLE:
                handle_idle(parsed_message, timed_message);
                break;
            case State::RUN:
                handle_run(parsed_message, timed_message);
                break;
            }
        }
        catch (const InvalidRawMessage& message)
        {
            // Ignora i messaggi invalidi
        }
    }

    if (current_state == State::RUN)
    {
        current_log_file.close();
        if (current_log_file.fail())
        {
            throw LogFileError();
        }

        std::filesystem::path stats_path = std::filesystem::path("output") /
            ("session_" + std::to_string(session_number) + "_" + current_session_timestamp + "_stats.csv");
        session_stats.save_csv_and_clear(stats_path.string());
    }
}

void TelemetryFSM::handle_idle(const ParsedMessage& msg, const TimedMessage& raw_msg)
{
    std::vector<ParsedMessage> start_codes = {
        ParsedMessage{160, std::vector<uint16_t>{102, 1}},
        ParsedMessage{160, std::vector<uint16_t>{255, 1}}
    };

    for (auto& start_code : start_codes)
    {
        if (msg == start_code)
        {
            // In TelemetryFSM::handle_idle:
            const std::filesystem::path output_dir = "output";
            std::error_code ec;
            std::filesystem::create_directories(output_dir, ec);
            if (ec)
            {\
                throw LogFileError();
            }

            current_session_timestamp = std::to_string(get_timestamp_ms());
            std::string base_name = "session_" + std::to_string(session_number) + "_" + current_session_timestamp;
            std::filesystem::path log_path = output_dir / (base_name + ".log");

            current_log_file.open(log_path);
            if (!current_log_file.is_open())
            {
                throw LogFileError();
            }

            current_state = State::RUN;
            handle_run(msg, raw_msg);
            return;
        }
    }
}

void TelemetryFSM::handle_run(const ParsedMessage& msg, const TimedMessage& raw_msg)
{
    auto sys_ms = std::chrono::duration_cast<std::chrono::milliseconds>(raw_msg.received_system.time_since_epoch()).
        count();

    current_log_file << sys_ms << " " << raw_msg.raw << std::endl;

    if (current_log_file.fail())
    {
        throw LogFileError();
    }
    // Delega i calcoli a update
    session_stats.update(msg.id, raw_msg.received_steady);

    std::vector<ParsedMessage> stop_codes = {
        ParsedMessage{160, std::vector<uint16_t>{102, 255}}
    };


    for (auto& stop_code : stop_codes)
    {
        if (msg == stop_code)
        {
            current_log_file.close();

            // CONTROLLO CHIUSURA
            if (current_log_file.fail())
            {
                throw LogFileError();
            }

            std::filesystem::path stats_path = std::filesystem::path("output") /
                ("session_" + std::to_string(session_number) + "_" + current_session_timestamp + "_stats.csv");
            session_stats.save_csv_and_clear(stats_path.string());
            session_number++;
            current_state = State::IDLE;
            return;
        }
    }
}
