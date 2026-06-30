#include "ServerNetworking.hpp"
#include "yavog/App.hpp"
#include "yavog/data/BinaryData.hpp"
#include "yavog/network/basic/SocketPoll.hpp"
#include "yavog/network/basic/TcpListener.hpp"
#include "yavog/network/basic/TcpSocket.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

void ServerNetworking::run(std::stop_token stoken,TcpListener listener){
    SocketPoll poll;
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
                
                addPlayer(con.con);

            }
            for(size_t i = 0;i<connections.size();){
                auto& con = connections[i];
                // closed
                if(poll.isClosed(con.client)){
                    if(con.client.exist()){
                        con.client.close();
                    }
                    con.con->isClose = true;
                    // swap remove
                    std::swap(connections[i], connections[connections.size()-1]);
                    connections.pop_back();
                    continue;
                }
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
                        con.receiving.insert(con.receiving.end(), buffer,buffer+received);
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
                }
                i++; 
            }
        }
    }
    listener.close();
}

bool ServerNetworking::listen(size_t port){
    if(networkThread.joinable()){
        networkThread.request_stop();
        networkThread.join();
    }

    TcpListener listener;

    auto portStr = std::to_string(port);
    if(!listener.listen(std::u8string(portStr.begin(),portStr.end())))
        return false;
    
    networkThread = std::jthread(&ServerNetworking::run,this, std::move(listener));
    return true;
}
time_t lastTime = 0;
void ServerNetworking::update(){
    std::lock_guard lock(mutex);
    for (size_t i = 0; i < players.size();) {
        auto& player = players[i];

        if(player.con->isClose){
            // swapremove
            std::cout << "player disconnected!"<<std::endl;
            removePlayer(player);
            std::swap(players[i], players[players.size()-1]);
            players.pop_back();
            continue;
        }
        i++;
    }


    for (size_t i = 0; i < players.size();i++) {
        auto& player = players[i];
        while(true){
            if(auto _bd = player.con->toServer.recv(); _bd.has_value()){
                auto bd = _bd.value();
                if(auto pi = procotolList.getProtocolInterface(bd)){
                    pi->serverReceive(player, bd);
                }
            }else break;
        }
    }
    
}
void ServerNetworking::addPlayer(std::shared_ptr<Connection> con){
    std::lock_guard lock(mutex);
    Player player;
    player.con = con;
    players.push_back(player);
    con->toClient.send(procotolList);
    

    procotolList.debugPrint.send(con->toClient, "Welcome!");
    auto playerEntity = App::app->entityManagerServer.addEntity();
    players.back().entity = playerEntity;
    for(auto& player:players)
        procotolList.entity.add(con->toClient, player.entity);
    for(auto& player:players)
        procotolList.entity.add(player.con->toClient, playerEntity);
    procotolList.chunk.send(con->toClient, *App::app->chunk);
}    

void ServerNetworking::removePlayer(Player& player){
    player.con->isClose = true;

    App::app->entityManagerServer.removeEntity(player.entity);
    for(auto& otherPlayer:players)
        procotolList.entity.remove(otherPlayer.con->toClient, player.entity);
}