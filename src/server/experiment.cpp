

#include "data/BinaryData.hpp"
#include "server/Client.hpp"
#include "server/Server.hpp"
#include <memory>
#include <stop_token>
#include <string>
#include <thread>

void client(std::stop_token token){
    ClientNetworkConnection cnc;
    cnc.join(U"localhost:5555");
    
    while(!token.stop_requested()){
        std::string str;
        std::getline(std::cin,str);
        std::u8string u8str(str.begin(),str.end());
        BinaryData bd;
        bd.writeString(u8str);
        cnc.con->toServer.send(bd);
        cnc.update();
        if(auto _bd = cnc.con->toClient.recv();_bd.has_value()){
            auto bd = _bd.value();
            std::u8string u8str;
            bd.readString(u8str);
            std::string str(u8str.begin(),u8str.end());
            std::cout << str << std::endl;
        }
    }

}
int experimentServer(){
    std::shared_ptr<Connection> host2 = std::make_shared<Connection>();
    {
        Server server;
        {
            server.addPlayer(host2);
        }
        server.listen(5555);
        
        std::jthread j(client);
        BinaryData bd;
        bd.writeString(u8"Hello world!");
        host2->toServer.send(bd);
        while(true){
            server.update();
            if(auto _bd = host2->toClient.recv(); _bd.has_value()){
                auto bd = _bd.value();
                std::u8string u8str;
                bd.readString(u8str);
                std::string str(u8str.begin(),u8str.end());
                std::cout << str << std::endl;
            }
        }
    }
    return 0;
}