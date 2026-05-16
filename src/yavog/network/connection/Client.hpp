#pragma  once
#include "yavog/data/BinaryData.hpp"
#include "yavog/network/basic/SocketPoll.hpp"
#include "yavog/network/basic/TcpListener.hpp"
#include "yavog/network/basic/TcpSocket.hpp"
#include "yavog/network/connection/Server.hpp"
#include <cstddef>
#include <cuchar>
#include <iostream>
#include <memory>
#include <sys/types.h>

struct ClientNetworkConnection{
    TcpSocket socket;
    SocketPoll poll;
    std::shared_ptr<Connection> con = std::make_shared<Connection>();
    
    std::vector<char> receiving;
    std::vector<char> sending;

    static const size_t BUFFER_SIZE = 2048;
    char buffer[BUFFER_SIZE];
                
    static std::u8string toUTF8(std::u32string u32address);
    bool join(std::u32string u32address);
    [[nodiscard]]bool update();
};

struct Client{
    ClientNetworkConnection cnc;
    ProtocolList pl;

    bool join(std::u32string u32address){
        if(!cnc.join(u32address))
            return false;
        while(cnc.update()){
            if(auto _bd = cnc.con->toClient.recv();_bd.has_value()){
                auto bd = _bd.value();
                if(!pl.createClient(bd)){
                    return false;
                }
                return true;
            }
        }
        return false;
    }
    // return: success?
    [[nodiscard]]bool update(){
        if(!cnc.update())
            return false;
        while(true){
            if(auto _bd = cnc.con->toClient.recv();_bd.has_value()){
                auto bd = _bd.value();
                if(auto pi = pl.getProtocolInterface(bd)){
                    pi->clientReceive(cnc.con->toServer, bd);
                }
            }else 
               break;
        }
        return true;
    }
};