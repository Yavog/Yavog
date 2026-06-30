#pragma once
#include <cstddef>
#include <memory>
#include <mutex>
#include <stop_token>
#include <thread>
#include <vector>
#include "yavog/network/basic/TcpListener.hpp"
#include "yavog/network/basic/TcpSocket.hpp"
#include "yavog/network/connection/ProtocolList.hpp"
#include "yavog/network/connection/Connection.hpp"




class ServerNetworking{
    //all of this  is owned by the network thread
    struct NetworkConnection{
        std::shared_ptr<Connection> con;
        TcpSocket client;
        
        std::vector<char> receiving;
        std::vector<char> sending;
    };

    void run(std::stop_token stoken,TcpListener listener);
    std::jthread networkThread;
    //this is owned by the update thread
    std::mutex mutex;
public:
    std::vector<Player> players;
    ProtocolList procotolList;

    [[nodiscard]] bool listen(std::size_t port);   
    void addPlayer(std::shared_ptr<Connection> con);    
    void removePlayer(Player& con);    
    void update();
};