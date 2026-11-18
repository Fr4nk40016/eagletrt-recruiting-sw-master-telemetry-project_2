//
// Created by francesco on 06/11/26.
//

#ifndef PROJECT2_RECEIVER_FUNCTION_H
#define PROJECT2_RECEIVER_FUNCTION_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>

// La firma della funzione che il thread eseguirà.
void receiver_function(std::stop_token stoken, std::queue<TimedMessage>& q, std::mutex& mtx,
                       std::condition_variable& cv, bool& finished);
#endif
