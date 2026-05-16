#pragma once
#include "yavog/data/BinaryData.hpp"
#include "yavog/network/connection/Channel.hpp"
#include <cstdint>
#include <iostream>

struct ProtocolInterface{
    uint16_t protocolId = -1;
    virtual void clientReceive(class Channel4TwoThread& responseChannel,BinaryData& input)=0;
    virtual void serverReceive(class Player& player,BinaryData& input)=0;
};

inline std::ostream& operator<<(std::ostream& os, const std::u8string& str) {
    return os.write(reinterpret_cast<const char*>(str.data()), str.size());
}
