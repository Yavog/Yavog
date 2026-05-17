#pragma  once
#include "yavog/entity/EntityManager.hpp"
#include "yavog/network/connection/Channel.hpp"
#include <cstdint>

struct Connection{
    Channel4TwoThread toServer;
    Channel4TwoThread toClient;

    std::atomic_bool isClose = false;
};
struct Player{
    std::shared_ptr<Connection> con;
    Entity entity;
};