#pragma once
#include <cstddef>
#include <functional>
#include <memory>
#include <vector>
#include "data/BinaryData.hpp"
#include "server/Player.hpp"

struct ConnectionInterface{
    virtual void send(const BinaryData& bd)=0;
    std::function<void(const BinaryData&)> receive;
};

class Server{
public:
    std::vector<ServerPlayer> players;


    void create();
    void listen(std::size_t port);

    void add(std::shared_ptr<ConnectionInterface> connectionInterface);

};