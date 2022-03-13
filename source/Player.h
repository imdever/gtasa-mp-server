class Player;

#pragma once
#include "Server.h"
#include "Packets.h"
#include "AbstractListener.h"
#include "AbstractConnection.h"
#include "iostream"
#include "string"

class Player: public AbstractConnection{
    int32_t ID = -1;
    int32_t CarID = -1;
    bool have_ped = false;
    PedInfo ped_info;
    std::thread* car_spawn_thread_ptr;
public:
    Player() = default;
    Player(AbstractConnection connection);

    int32_t getID();
    bool isHavePed();
    PedInfo getPedInfo();

protected:
    virtual void packetReceived(Packet packet) override;
    virtual void socketDisconnected();

    void broadcast_createPed();
    void broadcast_deletePed();
    void SpawnCar();

    void createAllServerPeds();
};
