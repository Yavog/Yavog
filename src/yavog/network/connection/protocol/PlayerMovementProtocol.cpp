#include "yavog/App.hpp"
#include "yavog/network/connection/ProtocolList.hpp"

void  PlayerMovementProtocol::serverReceive(Player& player,BinaryData& input){
    glm::vec3 position;
    if(input.readf32(position[0]) && input.readf32(position[1]) && input.readf32(position[2])){
        for(auto& otherPlayer:App::app->server.players){
            if(&otherPlayer!=&player)
                send(otherPlayer.con->toClient, position);
        }
    }
}
void PlayerMovementProtocol::clientReceive(Channel4TwoThread& con,BinaryData& input){
    glm::vec3 position;
    if(input.readf32(position[0]) && input.readf32(position[1]) && input.readf32(position[2])){
        // for (int i = 0; i < 3; i++) {
        //     std::cout << position[i]<<" ";
        // }
        // std::cout << std::endl;
        App::app->position = position;
    }
}