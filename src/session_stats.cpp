//
// Created by francesco on 07/11/26.
//

#include "../include/session_stats.h"

#include <iomanip>

#include "include/exceptions.h"

void SessionStats::update(uint16_t id, std::chrono::steady_clock::time_point current_timestamp) {
    if (stats_map.find(id) == stats_map.end()) {
        stats_map[id].message_count = 1;
        stats_map[id].cumulative_time_ms = 0.0;
        stats_map[id].last_timestamp = current_timestamp;
    } else {
        // La sottrazione tra steady_clocks è sicura al 100% e non underflowa mai
        auto delta = current_timestamp - stats_map[id].last_timestamp;

        // Convertiamo la differenza in millisecondi in virgola mobile per la media
        double delta_ms = std::chrono::duration<double, std::milli>(delta).count();

        stats_map[id].cumulative_time_ms += delta_ms;
        stats_map[id].message_count++;
        stats_map[id].last_timestamp = current_timestamp;
    }
}

void SessionStats::save_csv_and_clear(const std::string& filename) {
    std::ofstream stats_file(filename);

    // 1. Controllo Apertura
    if (!stats_file.is_open()) {
        throw CsvFileError();
    }

    stats_file << "ID,number_of_messages,mean_time\n";

    for (const auto& it : stats_map) {
        double mean_time = 0.0;
        if (it.second.message_count > 1) {
            mean_time = it.second.cumulative_time_ms / (it.second.message_count - 1.0);
        }
        stats_file << std::hex << std::uppercase << std::setw(3) << std::setfill('0') << it.first << std::dec << ","
                   << it.second.message_count << "," << mean_time << "\n";
    }

    // 2. Controllo Scrittura (se il disco si è riempito mentre scrivevi)
    if (stats_file.fail()) {
        throw CsvFileError();
    }

    // 3. Controllo Chiusura e Flush
    stats_file.close();
    if (stats_file.fail()) {
        throw CsvFileError();
    }

    stats_map.clear();
}