#include "Server.hpp"
#include "data/BinaryData.hpp"
#include "network/basic/SocketPoll.hpp"
#include "network/basic/TcpListener.hpp"
#include "network/basic/TcpSocket.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <vector>

void Server::run(std::stop_token stoken,size_t port){
    TcpListener listener;
    SocketPoll poll;
    
    auto portStr = std::to_string(port);
    listener.listen(std::u8string(portStr.begin(),portStr.end()));
    poll.add(listener);
        
    const size_t BUFFER_SIZE = 2048;
    char buffer[BUFFER_SIZE];
    std::vector<NetworkConnection> connections;
    while(!stoken.stop_requested()){
        // update poll for sendable sockets.
        for(size_t i = 0;i<connections.size();i++){
            auto& con = connections[i];
            if(!con.con->toClient.isEmpty()){
                poll.addWrite(con.client);
            }
        }
        if(poll.wait()){
            // new connection
            if(poll.isWriteable(listener)||poll.isReadable(listener)){
                std::string ipAddr;
                int port;
                
                connections.emplace_back();
                auto& con = connections[connections.size()-1];
                con.con = std::make_shared<Connection>();
                listener.accept(con.client,  ipAddr, port);
                poll.add(con.client,false,true);

                players.push_back({
                    con.con
                });
            }
            for(size_t i = 0;i<connections.size();){
                auto& con = connections[i];
                //send
                if(poll.isWriteable(con.client)){
                    if(con.sending.size() == 0){
                        if(auto _bd = con.con->toClient.recv();_bd.has_value()){
                            BinaryData bd;
                            bd.writeU32(_bd.value().getContent().size());
                            bd.writeBinaryData(_bd.value());
                            con.sending = bd.getContent();
                        }
                    }
                    if(con.sending.size()){                        
                        size_t transmitted = 0;
                        if(con.client.send(con.sending.data(), con.sending.size(),  transmitted)){
                            con.sending.erase(con.sending.begin(), con.sending.begin()+transmitted);
                        }
                    }
                    if(con.sending.size()==0)
                        poll.removeWrite(con.client);       
                }
                // receiving
                if (poll.isReadable(con.client)) {
                    size_t received;
                    if(con.client.recv(buffer, BUFFER_SIZE, received)){
                        con.receiving.insert(con.receiving.begin(), buffer,buffer+received);
                        size_t index = 0;
                        while(con.receiving.size() > 4+index){
                            BinaryData bd;
                            bd.writeBytes(std::vector<char>(con.receiving.begin()+index,con.receiving.begin()+4+index));
                            uint32_t size;
                            if(bd.readU32(size)){
                                if(con.receiving.size() >= index+4+size){
                                    BinaryData bd;
                                    bd.writeBytes(std::vector<char>(con.receiving.begin()+index+4,con.receiving.begin()+index+4+size));
                                    con.con->toServer.send(bd);
                                    index += size+4;
                                }else break;
                            }else break;
                        }
                        con.receiving.erase(con.receiving.begin(), con.receiving.begin()+index);
                    }
                    if(!con.client.exist()){
                        poll.remove(con.client);
                        con.con->isClose = true;
                        // swap remove
                        std::swap(connections[i], connections[connections.size()-1]);
                        connections.pop_back();
                        continue;
                    }
                }
                i++; 
            }
        }
    }
    listener.close();
}

void Server::listen(size_t port){
    networkThread = std::jthread(&Server::run,this, port);
}
void Server::update(){
    for (size_t i = 0; i < players.size();) {
        auto& player = players[i];

        if(player.con->isClose){
            // swapremove
            std::cout << "player disconnected!"<<std::endl;
            std::swap(players[i], players[players.size()-1]);
            players.pop_back();
            continue;
        }
        if(auto _bd = player.con->toServer.recv(); _bd.has_value())
        {
            auto bd = _bd.value();
            int8_t t;

            while(bd.readI8(t)){
                std::cout << t << std::endl;
            }
            player.con->toClient.send(bd);
        }
        i++;
    }
}
