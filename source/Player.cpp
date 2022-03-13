#include "Player.h"
using namespace std;

Player::Player(AbstractConnection connection): AbstractConnection(connection){
    car_spawn_thread_ptr = new thread([this] {
        SpawnCar();
    });
}

void Player::SpawnCar()
{
    SpawnCarPacket packet;
    packet.type = PacketType::SPAWN_CAR;
    packet.car_id = this->CarID;
    sendPacket(&packet, sizeof(SpawnCarPacket));
    return;
}

void Player::packetReceived(Packet packet){
    MultiplayerPacket *multiplayer_packet = reinterpret_cast<MultiplayerPacket*>(packet.packet_data);
    Server* server = static_cast<Server*>(listener_ptr);
    if( packet.packet_data == nullptr )
        return;
    int32_t id = -1;
    switch( multiplayer_packet->type ){
    case PacketType::REQUEST_ID:
        if( ID == -1 ) // Make sure that player already dont have ID
            id = server->requestId(this);
        if( id == -1 ){
            MultiplayerPacket packet;
            packet.type = PacketType::FAIL_TO_REQUEST_ID;
            sendPacket(&packet, sizeof(MultiplayerPacket));
            return;
        }
        cout << "Player connected [" << id << "]" << endl;

        YourIDPacket packet;
        packet.type = PacketType::YOU_ID;
        packet.user_id = id;
        sendPacket(&packet, sizeof(YourIDPacket));
        ID = id;
        broadcast_createPed();
        createAllServerPeds();
        break;
    case PacketType::UPDATE_PED:
        if( !isHavePed() ) // Make sure that player already have ped
            return;
        UpdatePedPacket* update_packet_ptr = static_cast<UpdatePedPacket*>(multiplayer_packet);
        this->ped_info = update_packet_ptr->info;
        break;
    case PacketType::UPDATE_CAR:
        UpdateCarPacket packetc;
        packetc.type = PacketType::UPDATE_CAR;
       // packetc.ped_id = this->ID;
        sendPacket(&packetc, sizeof(UpdateCarPacket));
        break;
    };

    delete[] static_cast<char*>(packet.packet_data);
}

void Player::socketDisconnected(){
    Server* server = static_cast<Server*>(listener_ptr);
    if( ID != -1 ){
        broadcast_deletePed();
        cout << "Player disconnected [" << ID << "]" << endl;
        server->freeID(ID);
        ID = -1;
    }
}

// Creating ped for all connected users
void Player::broadcast_createPed(){
    Server* server = static_cast<Server*>(listener_ptr);
    auto& connections = server->getConnections();
    for( AbstractConnection* connection: connections){
        if( connection == this )
            // skip broadcast sender connection
            continue;
        /*
         All users create PED and associate them with player ID.
         Player ID its player that would you this ped.
        */
        CreatePedPacket packet;
        packet.type   = PacketType::CREATE_PED;
        packet.ped_id = this->ID;
        connection->sendPacket(&packet, sizeof(CreatePedPacket));
    }
    have_ped = true;
}

// Delete ped for all connected users
void Player::broadcast_deletePed(){
    if( have_ped == false )
        return; // no PED for delete
    have_ped = false;
    Server* server = static_cast<Server*>(listener_ptr);
    auto& connections = server->getConnections();
    for( AbstractConnection* connection: connections){
        if( connection == this )
            // skip broadcast sender connection
            continue;
        /*
         All users delete PED associated with player ID.
         Player ID its player that use this ped before.
        */
        DeletePedPacket packet;
        packet.type   = PacketType::DELETE_PED;
        packet.ped_id = this->ID;
        connection->sendPacket(&packet, sizeof(DeletePedPacket));
    }
}

void Player::createAllServerPeds(){
    Server* server = static_cast<Server*>(listener_ptr);
    auto& connections = server->getConnections();
    for( AbstractConnection* connection: connections){
        if( connection == this )
            // skip this player
            continue;
        Player* other_player_ptr = static_cast<Player*>(connection);
        if( other_player_ptr->isHavePed() ){
            // this player have ped, well create ped of him
            CreatePedPacket packet;
            packet.type   = PacketType::CREATE_PED;
            packet.ped_id = other_player_ptr->getID();
            this->sendPacket(&packet, sizeof(CreatePedPacket));
        }
    }
}

PedInfo Player::getPedInfo(){
    return ped_info;
}

int32_t Player::getID(){
    return ID;
}

bool Player::isHavePed(){
    return have_ped;
}
