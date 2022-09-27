#include "game_network/game_settings_server.h"

#include "game/default_settings.h"

#include <iostream>
#include <stdlib.h>
#include <time.h>

#define INTERVAL 100

int get_device_address(ENetAddress* out){
	int ret = 1;
	
	ENetSocket s = enet_socket_create(ENET_SOCKET_TYPE_DATAGRAM);
	if(s == ENET_SOCKET_NULL){
		return 1;
	}

	ENetAddress a;
	a.port = 80;
	if(enet_address_set_host(&a, "8.8.8.8")) goto out;
	
	if(enet_socket_connect(s, &a)) goto out;
	
	if(enet_socket_get_address(s, out)) goto out;
	
	ret = 0;
out:
	enet_socket_destroy(s);
	
	return ret;
}

#define BUFFER_LEN 1024

int main(int argc, char** argv){
	srand(time(NULL));

	if(enet_initialize() != 0){
		cerr << "Error while initializing enet" << endl;
		return 1;
	}
	atexit(enet_deinitialize);
	
	ENetAddress address;
	char server_name[BUFFER_LEN];
	
	if(get_device_address(&address)) address.host = ENET_HOST_ANY;
	
	if(
		enet_address_get_host(&address, server_name, BUFFER_LEN) &&
		enet_address_get_host_ip(&address, server_name, BUFFER_LEN)
	) server_name[0] = '\0';
	
	cout << server_name << endl;
	
	GameSettingsServer(default_settings).serve(INTERVAL);
}
