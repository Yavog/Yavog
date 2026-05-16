#include "yavog/network/connection/ProtocolList.hpp"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string_view>

ProtocolList::ProtocolList(){
    for(auto& pair:mapping)
        reverseMapping[pair.second] = pair.first;
    for(auto& pair:mapping){
        pair.second->protocolId = sorted.size(); 
        sorted.push_back(pair.second);
    }
}


ProtocolList::operator BinaryData(){
    BinaryData rv;
    for(auto& protocol:sorted){
        rv.writeString(reverseMapping[protocol]);
    }
    return rv;
} 
[[nodiscard]] bool ProtocolList::createClient(BinaryData& protocolPalette){
    std::u8string key;
    sorted.clear();
    while (protocolPalette.readString(key)) {
        if(mapping.find(key)==mapping.end()){
            std::cerr << "Unknown protocol:"<< std::string_view(reinterpret_cast<const char*>(key.data()),key.size()) << std::endl;
            return false;
        }
        mapping[key]->protocolId = sorted.size(); 
        sorted.push_back(mapping[key]);
    }
    return true;
}
ProtocolInterface* ProtocolList::getProtocolInterface(BinaryData& input){
    uint16_t protocolId;
    input.readU16(protocolId);

    if(sorted.size() <= protocolId){
        return nullptr;
    }
    return sorted[protocolId];
}
