#pragma once
#include "yavog/entity/EntityManager.hpp"
#include "yavog/network/connection/ProtocolInterface.hpp"
#include "yavog/network/connection/Connection.hpp"
#include <cstdint>
#include <string>

struct EntityProtocol:public ProtocolInterface{
    enum CommandType{
        ADD = 0,
        REMOVE = 1
    };

    virtual void clientReceive(Channel4TwoThread& con,BinaryData& input)override;
    virtual void serverReceive(Player& player,BinaryData& input)override;

    void add(Channel4TwoThread& con,Entity id);
    void remove(Channel4TwoThread& con,Entity id);
};