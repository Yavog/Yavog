#include "Server.hpp"
#include "data/BinaryData.hpp"
#include "network/basic/SocketPoll.hpp"
#include "network/basic/TcpListener.hpp"
#include <chrono>
#include <cstddef>
#include <iostream>
#include <optional>
#include <string>
#include <thread>

void Server::create(){
    thread = std::jthread(&Server::run,this, std::nullopt);
}
void Server::run(std::stop_token stoken,std::optional<size_t> port){
    TcpListener listener;
    SocketPoll poll;
    
    bool isOpen = port.has_value();
    
    if(isOpen){
        auto portStr = std::to_string(port.value());
        listener.listen(std::u8string(portStr.begin(),portStr.end()));
        poll.add(listener);
    }
        
    TcpSocket client;
    while(!stoken.stop_requested()){
        if(isOpen){
            if(poll.wait(0)){
                if(poll.isWriteable(listener)||poll.isReadable(listener)){
                    std::string ipAddr;
                    int port;
                    listener.accept(client,  ipAddr, port);
                    poll.add(client,false,true);
                }
                if (poll.isReadable(client)) {
                
                    char buffer[1000];
                    size_t received;
                    if(client.recv(buffer, 1000, received)){
                        std::cout << std::string(buffer,buffer+received)<<std::endl;
                    }
                }
            }
        }
        std::cout << "."<<std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    if(isOpen)
        listener.close();
}

void Server::listen(size_t port){
    thread = std::jthread(&Server::run,this, port);
}
void Server::add(std::shared_ptr<ConnectionInterface> connectionInterface){
    connectionInterface->receive = [](BinaryData bd){
        std::u8string str;
        bd.readString(str);
        std::cout << "received: "<< (const char*)str.c_str()<< std::endl;
    };
    BinaryData bd;
    bd.writeString(u8"hello there");
    connectionInterface->send(bd);    
}
