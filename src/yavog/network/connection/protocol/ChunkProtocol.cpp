#include "App.hpp"
#include "network/connection/ProtocolList.hpp"
#include "world/Chunk.hpp"
#include <memory>

void ChunkProtocol::clientReceive(Channel4TwoThread& con,BinaryData& input){
    App::app->chunk = std::make_shared<Chunk>(input);
}
void ChunkProtocol::send(Channel4TwoThread& con,Chunk& chunk){
    BinaryData bd;
    bd.writeU16(protocolId);
    bd.writeBinaryData(chunk);
    con.send(bd);
}