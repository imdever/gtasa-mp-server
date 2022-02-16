#pragma once
#include "Player.h"
#include "AbstractListener.h"
#include "AbstractConnection.h"
#include "iostream"
#include "string"
using namespace std;

class Server: public AbstractListener {
    Player** players_array;
    std::thread* server_sync_thread_ptr;
    bool delete_later = false;
    
public:
    Server();
    ~Server() = default;

    void freeID(int32_t id);
    int32_t requestId(Player* player);
    void syncHandler();

protected:
    void newConnection(SOCKET connection_socket ) override;
};