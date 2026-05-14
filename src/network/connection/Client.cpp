#include "Client.hpp"

std::u8string ClientNetworkConnection::toUTF8(std::u32string u32address)
{
    std::u8string u8address;

    setlocale(LC_ALL, "en_US.utf8");
    char buffer[MB_CUR_MAX];
    std::mbstate_t state{}; 
            
    for (auto& c32: u32address) {
        if(size_t rc = std::c32rtomb(buffer, c32, &state))
        {
            if (rc == (std::size_t) - 1)
                break;
            if (rc == (std::size_t) - 2)
                break;
            u8address += std::u8string(buffer,buffer+rc);
        }
    }
    return u8address;
}
bool ClientNetworkConnection::join(std::u32string u32address){
    //convert u32 to u8string:
    std::u8string u8address = toUTF8(u32address);
    
    size_t split = u8address.find(':');
    std::u8string ip = u8address.substr(0,split); 
    std::u8string port = u8"5555";  
    if(split!=std::string::npos){
        port = u8address.substr(split+1);
    }
    if(!socket.connect(ip, port)){
        std::cout << "failed to connect to "<<(char*)ip.c_str()<<" "<<(char*)port.c_str() << std::endl;
        return false;
    }
    poll.add(socket,false,true);
    return true;
}
void ClientNetworkConnection::update(){
    if(!socket.exist()){
        con->isClose = true;
        return;
    }
    //send 
    if(!con->toServer.isEmpty()){
        poll.addWrite(socket);
    }
    
    if(poll.wait()){
        //send
        if(poll.isWriteable(socket)){
            if(!sending.size()){
                if(auto _bd = con->toServer.recv();_bd.has_value()){
                    BinaryData bd;
                    bd.writeU32(_bd.value().getContent().size());
                    bd.writeBinaryData(_bd.value());
                    sending = bd.getContent();
                }
            }
            if(sending.size()){
                size_t transmitted;

                if(socket.send(sending.data(), sending.size(),  transmitted)){
                    sending.erase(sending.begin(), sending.begin()+transmitted);
                }
            }
            if(!sending.size())
                poll.removeWrite(socket);
        }
        //recv
        if(poll.isReadable(socket)){
            size_t received;
            if(socket.recv(buffer, BUFFER_SIZE, received)){
                receiving.insert(receiving.begin(), buffer,buffer+received);
                size_t index = 0;
                while(receiving.size() > 4+index){
                    BinaryData bd;
                    bd.writeBytes(std::vector<char>(receiving.begin()+index,receiving.begin()+4+index));
                    uint32_t size;
                    if(bd.readU32(size)){
                        if(receiving.size() >= index+4+size){
                            BinaryData bd;
                            bd.writeBytes(std::vector<char>(receiving.begin()+index+4,receiving.begin()+index+4+size));
                            con->toClient.send(bd);
                            index += size+4;
                        }else break;
                    }else break;
                }
                receiving.erase(receiving.begin(), receiving.begin()+index);
            }
        }
    }
}