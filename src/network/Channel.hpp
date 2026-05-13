#pragma once

#include "data/BinaryData.hpp"
#include <array>
#include <atomic>
#include <optional>

const int cachline = 64; 
const int capacity = 1024;

class Channel4TwoThread{
public:
    std::array<BinaryData, capacity> buffer;

    alignas(cachline) std::atomic_uint16_t head{0};
    alignas(cachline) std::atomic_uint16_t tail{0};

    bool send(const BinaryData& bd);
    std::optional<BinaryData> recv();

    bool isEmpty();
};