#include "network/connection/ProtocolInterface.hpp"
#include "network/connection/Connection.hpp"
#include <string>

struct DebugPrintProtocol:public ProtocolInterface{
    virtual void clientReceive(Channel4TwoThread& con,BinaryData& input){
        std::u8string str;
        input.readString(str);
        std::cout << "[Server]"<< str <<std::endl;
    }
    virtual void serverReceive(Channel4TwoThread& con,BinaryData& input){
        std::u8string str;
        input.readString(str);
        std::cout << "[Client]"<< str <<std::endl;
    }

    void send(Channel4TwoThread& con,std::string msg){
        send(con,std::u8string(msg.begin(),msg.end()));
    }
    void send(Channel4TwoThread& con,std::u8string msg){
        BinaryData bd;
        bd.writeU16(protocolId);
        bd.writeString(msg);
        con.send(bd);
    }
};