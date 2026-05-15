#include "network/connection/ProtocolInterface.hpp"
#include <cassert>
#include <string>

struct ChunkProtocol:public ProtocolInterface{
    virtual void clientReceive(Channel4TwoThread& con,BinaryData& input)override;
    virtual void serverReceive(Channel4TwoThread& con,BinaryData& input)override{
    }
    void send(Channel4TwoThread& con,class Chunk& chunk);
};