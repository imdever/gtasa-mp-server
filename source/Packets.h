#pragma once
#include <stdint.h>

#pragma pack(push, 1)
enum class PacketType {
    REQUEST_ID, FAIL_TO_REQUEST_ID, YOU_ID,
    CREATEM_PLAYER, DELETE_PLAYER,
    CREATE_PED, DELETE_PED, UPDATE_PED, SPAWN_CAR, UPDATE_CAR
};

struct Vector3d {
    double x = 2495.330078, y = -1676.422485, z = 13.337957;
};

struct PedInfo {
    Vector3d pos;
    float currot;
};

struct MultiplayerPacket {
    PacketType type;
};

struct SpawnCarPacket : public MultiplayerPacket {
    int32_t car_id;
    //PedInfo info;
};

struct UpdateCarPacket : public MultiplayerPacket {
    //int32_t ped_id;
    //PedInfo info;
};

struct YourIDPacket : public MultiplayerPacket {
    int32_t user_id;
};

struct CreatePedPacket : public MultiplayerPacket {
    int32_t ped_id;
};

struct UpdatePedPacket : public MultiplayerPacket {
    int32_t ped_id;
    PedInfo info;
};

struct DeletePedPacket : public MultiplayerPacket {
    int32_t ped_id;
};
#pragma pack(pop)
