#include "glm/ext/vector_float3.hpp"
#include "yavog/client/Camera.hpp"
#include "yavog/network/connection/ProtocolInterface.hpp"
#include "yavog/network/connection/Connection.hpp"
#include <string>

struct PlayerMovementProtocol:public ProtocolInterface{
    virtual void clientReceive(Channel4TwoThread& con,BinaryData& input)override;
    virtual void serverReceive(Player& player,BinaryData& input)override;

    void sendServer(Channel4TwoThread& con,Camera& camera){
        BinaryData bd;
        bd.writeU16(protocolId);
        bd.writef32(camera.pos.x);
        bd.writef32(camera.pos.y);
        bd.writef32(camera.pos.z);
        bd.writef32(camera.phi);
        bd.writef32(camera.theta);
        con.send(bd);
    }
    void sendClient(Channel4TwoThread& con,Player& player,glm::vec3 pos,float phi,float theta){
        BinaryData bd;
        bd.writeU16(protocolId);
        bd.writeU32(player.entity);
        bd.writef32(pos.x);
        bd.writef32(pos.y);
        bd.writef32(pos.z);
        bd.writef32(phi);
        bd.writef32(theta);

        con.send(bd);
    }
};