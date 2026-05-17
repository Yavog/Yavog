#include "yavog/App.hpp"
#include "yavog/entity/EntityManager.hpp"
#include "yavog/network/connection/ProtocolList.hpp"

void  PlayerMovementProtocol::serverReceive(Player& player,BinaryData& input){
    glm::vec3 position;
    float phi;
    float theta;
    if(input.readf32(position[0]) && input.readf32(position[1]) && input.readf32(position[2])&& input.readf32(phi) && input.readf32(theta)){
        for(auto& otherPlayer:App::app->server.players){
            if(&otherPlayer!=&player)
                sendClient(otherPlayer.con->toClient,player, position,phi,theta);
        }
    }
}
void PlayerMovementProtocol::clientReceive(Channel4TwoThread& con,BinaryData& input){
    glm::vec3 position;
    float phi;
    float theta;
    Entity entity;
    if(input.readU32(entity) &&input.readf32(position[0]) && input.readf32(position[1]) && input.readf32(position[2])&& input.readf32(phi) && input.readf32(theta)){
        // for (int i = 0; i < 3; i++) {
        //     std::cout << position[i]<<" ";
        // }
        // std::cout << std::endl;
        if(auto _comp = App::app->entityPositions.get(entity);_comp.has_value()){
            auto comp = _comp.value();
            *comp = position;
        }
        App::app->phi = phi;
        App::app->theta = theta;
        
    }
}