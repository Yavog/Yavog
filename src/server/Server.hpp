#pragma once
#include <atomic>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
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
    //all of this  is owned by the network thread
    struct NetworkConnection{
        std::shared_ptr<Connection> con;
        TcpSocket client;
        
        std::vector<char> receiving;
        std::vector<char> sending;
    };

    void run(std::stop_token stoken,size_t port);
    std::jthread networkThread;
    //this is owned by the update thread
    std::vector<Player> players;
    std::mutex mutex;
public:

    void listen(std::size_t port);   
    void addPlayer(std::shared_ptr<Connection> con);    
    void update();
};