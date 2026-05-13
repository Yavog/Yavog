#include "network/Channel.hpp"
#include "data/BinaryData.hpp"
#include <atomic>
#include <cstdint>
#include <optional>

bool Channel4TwoThread::send(const BinaryData& bd){
    uint16_t currentTail = tail.load(std::memory_order_relaxed);
    uint16_t currentHead = head.load(std::memory_order_acquire);

    if((currentTail+1)%capacity == currentHead)
        return false;

    buffer[currentTail] = bd;
    buffer[currentTail].resetHead();
    
    tail.store((currentTail+1)%capacity,std::memory_order_release);
    return true;
}

std::optional<BinaryData> Channel4TwoThread::recv(){
    uint16_t currentHead = head.load(std::memory_order_relaxed);
    uint16_t currentTail = tail.load(std::memory_order_acquire);

    if(currentTail == currentHead)
        return std::nullopt;

    auto bd = std::move(buffer[currentHead]);
    head.store((currentHead+1)%capacity,std::memory_order_release);
    return std::move(bd);
}

bool Channel4TwoThread::isEmpty(){
    uint16_t currentHead = head.load(std::memory_order_relaxed);
    uint16_t currentTail = tail.load(std::memory_order_relaxed);

    return (currentTail == currentHead);
}
