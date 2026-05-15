#pragma once
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "data/BinaryData.hpp"
#include "network/connection/ProtocolInterface.hpp"
#include "network/connection/protocol/DebugPrintProtocol.hpp"
#include "network/connection/protocol/ChunkProtocol.hpp"

class ProtocolList{
public:
    DebugPrintProtocol debugPrint;
    ChunkProtocol      chunk;
private:
    std::map<std::u8string, ProtocolInterface*> mapping = {
        {u8"debugPrint.0",&debugPrint},
        {u8"chunk.0",&chunk}
    };
    std::map<ProtocolInterface*,std::u8string> reverseMapping;

public:


    std::vector<ProtocolInterface*> sorted;

    ProtocolList(ProtocolList&)=delete;
    ProtocolList operator=(ProtocolList&)=delete;

    ProtocolList();

    operator BinaryData();
    [[nodiscard]] bool createClient(BinaryData& protocolPalette);

    bool receive(class Connection& con,BinaryData& input, bool clientSide);
};