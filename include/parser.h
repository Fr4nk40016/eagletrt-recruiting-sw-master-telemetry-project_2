//
// Created by francesco on 07/11/26.
//

#ifndef PROJECT2_PARSER_H
#define PROJECT2_PARSER_H

#include <string>
#include <vector>
#include <cstdint>
#include <stdexcept>

struct ParsedMessage
{
    uint16_t id;
    std::vector<uint16_t> payload;

    bool operator==(const ParsedMessage&) const = default;
};

namespace Parser
{
    bool raw_id_checker(const std::string& raw_id);
    bool raw_payload_checker(const std::string& raw_payload);
    ParsedMessage parse(const std::string& raw_msg);
}


#endif //PROJECT2_PARSER_H
