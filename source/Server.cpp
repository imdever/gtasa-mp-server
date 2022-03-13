#include "Server.h"

Server::Server(): AbstractListener(){
    players_array = new Player*[1024];
    memset(players_array, 0x00, sizeof(Player*) * 1024);
   server_sync_thread_ptr = new thread([this] {
            while (!delete_later) {
                std::this_thread::sleep_for(std::chrono::microseconds(16000));
                syncHandler();
            }
            delete this;
        });
    server_sync_thread_ptr->detach();
}

void Server::newConnection(SOCKET connection_socket ){
    AbstractConnection new_connection(this, connection_socket);
    Player* player_connection = new Player(new_connection);
    connections_list.push_back(player_connection);
    connections_count++;
    player_connection->startReading();
}

int32_t Server::requestId(Player* player){
    lockCriticalSection();
    for( int32_t i = 0; i < 1024; i++ ){
        if( players_array[i] == nullptr ){
            players_array[i] = player;
            unlockCriticalSection();
            return i;
        }
    }
    unlockCriticalSection();
    return -1;
}

void Server::freeID(int32_t id){
    lockCriticalSection();
    players_array[id] = nullptr;
    unlockCriticalSection();
}

void Server::syncHandler(){
    lockCriticalSection();
    for( AbstractConnection* connection_first : connections_list ){
        Player* player_first = static_cast<Player*>(connection_first);
        if( !player_first->isHavePed() )
            continue;
        UpdatePedPacket packet;
        packet.type   = PacketType::UPDATE_PED;
        packet.ped_id = player_first->getID();
        packet.info   = player_first->getPedInfo();
        for( AbstractConnection* connection_second : connections_list ){
            Player* player_second = static_cast<Player*>(connection_second);
            if( player_second->getID() == -1 )
                continue;
            if( player_first == player_second )
                continue;
            player_second->sendPacket(&packet, sizeof(UpdatePedPacket));
        }
    }
    unlockCriticalSection();
}

void Server::lockCriticalSection(){
    critical_section_mutex.lock();
}

void Server::unlockCriticalSection(){
    critical_section_mutex.unlock();
}
