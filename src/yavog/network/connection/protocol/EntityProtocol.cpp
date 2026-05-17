#include "yavog/App.hpp"
#include "yavog/network/connection/protocol/EntityProtocol.hpp"

void EntityProtocol::clientReceive(Channel4TwoThread& con,BinaryData& input){
    uint8_t t;
    input.readU8(t);
    CommandType type = (CommandType)t;
    uint32_t id;
    input.readU32(id);

    if(type == CommandType::ADD){
        App::app->entityMovement.insert(id);
    }
    else if(type == CommandType::REMOVE){
        App::app->entityMovement.erase(id);
    }
}
void EntityProtocol::serverReceive(Player& player,BinaryData& input){
}
void EntityProtocol::add(Channel4TwoThread& con,Entity id){
    BinaryData bd;
    bd.writeU16(protocolId);
    bd.writeU8(CommandType::ADD);
    bd.writeU32(id);
    con.send(bd);
}
void EntityProtocol::remove(Channel4TwoThread& con,Entity id){
    BinaryData bd;
    bd.writeU16(protocolId);
    bd.writeU8(CommandType::REMOVE);
    bd.writeU32(id);
    con.send(bd);
}