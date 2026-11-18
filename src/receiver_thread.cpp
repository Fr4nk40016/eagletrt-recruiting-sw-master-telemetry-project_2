// src/receiver_thread.cpp
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <stop_token> // Aggiungi
#include <thread>     // Aggiungi per std::this_thread::sleep_for

#include "../include/telemetry_fsm.h"

extern "C" {
#include "../include/fake_receiver.h"
}

// 1. Aggiungi stoken
void receiver_function(std::stop_token stoken, std::queue<TimedMessage>& q, std::mutex& mtx,
                       std::condition_variable& cv, bool& finished)
{
    char buffer[4096] = {0};

    // 2. Gira finché nessuno ha chiesto di fermarsi
    while (!stoken.stop_requested())
    {
        int bytes_read = can_receive(buffer);

        if (bytes_read > 0)
        {
            auto sys_now = std::chrono::system_clock::now();
            auto steady_now = std::chrono::steady_clock::now();

            std::string data(buffer, bytes_read);
            while (!data.empty() && (data.back() == '\r' || data.back() == '\n'))
            {
                data.pop_back();
            }

            if (!data.empty())
            {
                TimedMessage msg;
                msg.raw = data;
                msg.received_system = sys_now;
                msg.received_steady = steady_now;

                {
                    std::lock_guard<std::mutex> lock(mtx);
                    q.push(msg);
                }
                cv.notify_one();
            }
        }
        else if (bytes_read == -1)
        {
            // Esci semplicemente dal ciclo
            break;
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    // 3. Segnala la fine delle operazioni (sia per errore -1, sia per stop richiesto)
    {
        std::lock_guard<std::mutex> lock(mtx);
        finished = true;
    }
    cv.notify_one();
}
