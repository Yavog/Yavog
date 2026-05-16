#include "glm/ext/vector_float3.hpp"
#include "yavog/client/Camera.hpp"
#include "yavog/network/connection/ProtocolInterface.hpp"
#include "yavog/network/connection/Connection.hpp"
#include <string>

struct PlayerMovementProtocol:public ProtocolInterface{
    virtual void clientReceive(Channel4TwoThread& con,BinaryData& input)override;
    virtual void serverReceive(Player& player,BinaryData& input)override;

    void send(Channel4TwoThread& con,Camera& camera){
        BinaryData bd;
        bd.writeU16(protocolId);
        bd.writef32(camera.pos.x);
        bd.writef32(camera.pos.y);
        bd.writef32(camera.pos.z);
        con.send(bd);
    }
    void send(Channel4TwoThread& con,glm::vec3 pos){
        BinaryData bd;
        bd.writeU16(protocolId);
        bd.writef32(pos.x);
        bd.writef32(pos.y);
        bd.writef32(pos.z);
        con.send(bd);
    }
};