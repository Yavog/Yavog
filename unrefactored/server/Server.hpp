#pragma once
#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <stop_token>
#include <thread>
#include <vector>
#include "data/BinaryData.hpp"
#include "server/Player.hpp"

struct ConnectionInterface{
    virtual void send(const BinaryData& bd)=0;
    std::function<void(const BinaryData&)> receive;
};

class Server{
    void run(std::stop_token stoken,std::optional<size_t> port);
    std::jthread thread;
public:
    std::vector<ServerPlayer> players;

    
    void create();
    void listen(std::size_t port);

    void add(std::shared_ptr<ConnectionInterface> connectionInterface);
    
};