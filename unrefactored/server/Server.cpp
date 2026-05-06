#include "Server.hpp"
#include "data/BinaryData.hpp"
#include <cstddef>
#include <iostream>
#include <string>

void Server::create(){

}
void Server::listen(size_t port){
    
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
