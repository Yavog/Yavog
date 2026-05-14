#pragma once
#include <cstddef>
#include <memory>
#include <mutex>
#include <stop_token>
#include <thread>
#include <vector>
#include "network/basic/TcpSocket.hpp"
#include "network/connection/ProtocolList.hpp"
#include "network/connection/Connection.hpp"


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
    ProtocolList procotolList;

    void listen(std::size_t port);   
    void addPlayer(std::shared_ptr<Connection> con);    
    void update();
};