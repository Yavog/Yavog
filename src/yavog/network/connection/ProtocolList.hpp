#pragma once
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "yavog/data/BinaryData.hpp"
#include "yavog/network/connection/ProtocolInterface.hpp"
#include "yavog/network/connection/protocol/DebugPrintProtocol.hpp"
#include "yavog/network/connection/protocol/ChunkProtocol.hpp"
#include "yavog/network/connection/protocol/PlayerMovementProtocol.hpp"

class ProtocolList{
public:
    DebugPrintProtocol     debugPrint;
    ChunkProtocol          chunk;
    PlayerMovementProtocol playerMovement;
private:
    std::map<std::u8string, ProtocolInterface*> mapping = {
        {u8"debugPrint.0",&debugPrint},
        {u8"chunk.0",&chunk},
        {u8"playerMovement.0",&playerMovement}
    };
    std::map<ProtocolInterface*,std::u8string> reverseMapping;

public:


    std::vector<ProtocolInterface*> sorted;

    ProtocolList(ProtocolList&)=delete;
    ProtocolList operator=(ProtocolList&)=delete;

    ProtocolList();

    operator BinaryData();
    [[nodiscard]] bool createClient(BinaryData& protocolPalette);

    ProtocolInterface* getProtocolInterface(BinaryData& input);
};