#pragma  once
#include "network/connection/Channel.hpp"

struct Connection{
    Channel4TwoThread toServer;
    Channel4TwoThread toClient;

    std::atomic_bool isClose = false;
};
