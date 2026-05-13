#pragma once
#include <atomic>
#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <stop_token>
#include <thread>
#include <vector>
#include "data/BinaryData.hpp"
#include "network/Channel.hpp"
#include "network/basic/TcpSocket.hpp"
#include "server/Player.hpp"

struct Connection{
    Channel4TwoThread toServer;
    Channel4TwoThread toClient;

    std::atomic_bool isClose = false;
};

struct Player{
    std::shared_ptr<Connection> con;
};

class Server{
    struct NetworkConnection{
        std::shared_ptr<Connection> con;
        TcpSocket client;
        
        std::vector<char> receiving;
        std::vector<char> sending;
    };

    void run(std::stop_token stoken,size_t port);
    std::jthread networkThread;
public:
    std::vector<Player> players;

    void listen(std::size_t port);   
    
    void update();
};