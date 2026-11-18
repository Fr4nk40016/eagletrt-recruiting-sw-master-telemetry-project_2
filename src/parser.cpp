//
// Created by francesco on 07/11/26.
//

#include "../include/parser.h"

#include <algorithm>

#include "include/exceptions.h"

namespace Parser
{
    bool raw_id_checker(const std::string& raw_id)
    {
        if (raw_id.empty() || raw_id.length() > 3) return false;

        return std::all_of(raw_id.begin(), raw_id.end(), [](unsigned char c)
        {
            return std::isxdigit(c);
        });
    }

    bool raw_payload_checker(const std::string& raw_payload)
    {
        bool length_condition = raw_payload.length() % 2 == 0 && raw_payload.length() <= 16;

        bool hex_digit_condition = std::all_of(raw_payload.begin(), raw_payload.end(), [](unsigned char c)
        {
            return std::isxdigit(c);
        });

        return hex_digit_condition && length_condition;
    }

    ParsedMessage parse(const std::string& raw_msg)
    {
        size_t pos = raw_msg.find('#');
        if (pos == std::string::npos)
        {
            throw InvalidRawMessage();
        }

        std::string raw_id = raw_msg.substr(0, pos);
        std::string raw_payload = raw_msg.substr(pos + 1);
        std::vector<uint16_t> parsed_payload;

        if (raw_id_checker(raw_id) && raw_payload_checker(raw_payload))
        {
            for (size_t i = 0; i < raw_payload.length(); i += 2)
            {
                parsed_payload.push_back(std::stoi(raw_payload.substr(i, 2), nullptr, 16));
            }
        }
        else
        {
            throw InvalidRawMessage();
        }

        ParsedMessage parsed_message;
        parsed_message.id = std::stoi(raw_id, nullptr, 16);
        parsed_message.payload = parsed_payload;
        return parsed_message;
    }
}
