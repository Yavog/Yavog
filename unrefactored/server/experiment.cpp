

#include "data/BinaryData.hpp"
#include "server/Server.hpp"
#include <memory>

int experimentServer(){
    Server server;
    server.create();
    server.listen(5555);
    struct Test:public ConnectionInterface{
        virtual void send(const BinaryData& bd)override{
            this->receive(bd);
        }
    };
    server.add(std::make_shared<Test>());
    return 0;
}