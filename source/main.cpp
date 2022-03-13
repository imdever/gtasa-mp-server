#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "Server.h"
using namespace std;


int main(){
    try{
        Server server;
        server.setListenerParams(ListenerParams{7777, "192.168.0.11"});
        server.startListening();
    }catch(string error_message){
        cout << error_message;
    }
    while(true);
    return 0;
}
