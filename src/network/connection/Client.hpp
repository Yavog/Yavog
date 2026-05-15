#pragma  once
#include "data/BinaryData.hpp"
#include "network/basic/SocketPoll.hpp"
#include "network/basic/TcpListener.hpp"
#include "network/basic/TcpSocket.hpp"
#include "network/connection/Server.hpp"
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
    bool firstPackage;

    bool join(std::u32string u32address){
        if(!cnc.join(u32address))
            return false;
        firstPackage = true;
        return true;
    }
    // return: success?
    [[nodiscard]]bool update(){
        if(!cnc.update())
            return false;

        if(auto _bd = cnc.con->toClient.recv();_bd.has_value()){
            auto bd = _bd.value();
            
            if(firstPackage){
                if(!pl.createClient(bd)){
                    return false;
                }
            }else{
                pl.receive(*cnc.con, bd, true);
            }
            firstPackage = false;
        }
        return true;
    }
};