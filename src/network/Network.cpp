/*
 * Network.cpp
 *
 *  Created on: Aug 16, 2021
 *      Author: exuvo
 */

#include <iostream>
#include <fmt/core.h>

#include "Network.hpp"

//#include "slikenet/types.h"
#include "slikenet/BitStream.h"
#include "slikenet/MessageIdentifiers.h"
#include "slikenet/GetTime.h"

Network::Network(): server(SLNet::RakPeerInterface::GetInstance()) {
	//TODO not needed?
	server->GetNumberOfAddresses();
	
	uint16_t port = 6112;
	SLNet::SocketDescriptor socketDescriptor(port, 0);
	socketDescriptor.socketFamily = AF_INET;
	
	//TODO windows THREAD_PRIORITY_NORMAL
	int threadPrio = 20;
	
	int err = server->Startup(MAX_CONNECTIONS, &socketDescriptor, 1, threadPrio);
	
	while (err == SLNet::SOCKET_PORT_ALREADY_IN_USE && socketDescriptor.port < port + 10) {
		std::cout << "Port " << socketDescriptor.port << " already in use, trying next" << std::endl << std::flush;
		socketDescriptor.port++;
		err = server->Startup(MAX_CONNECTIONS, &socketDescriptor, 1, threadPrio);
	}
	
	if (err == SLNet::RAKNET_STARTED) {
		server->SetMaximumIncomingConnections(2);
		
		std::cout << "Listening on " << socketDescriptor.port << " for connections" << std::endl << std::flush;
		LOG4CXX_INFO(log, "Listening on " << socketDescriptor.port << " for connections");
	} else {
		std::cout << "Unable to listen on port " << socketDescriptor.port << ", SLNet StartupResult " << err << std::endl << std::flush;
		LOG4CXX_ERROR(log, "Unable to listen on port " << socketDescriptor.port << ", SLNet StartupResult " << err);
	}
	
	if (socketDescriptor.port != port) {
		std::cout << "pinging" << std::endl << std::flush;
		server->Ping("127.0.0.1", port, false);
	}
}

void Network::receive() {
	if (!server->IsActive()) {
		return;
	}
	
	SLNet::Packet* p;
	
	while((p = server->Receive()) != nullptr) {
		std::cout << "got network data " << (int) p->data[0] << std::endl << std::flush;
		
		switch (p->data[0]) {
			case ID_UNCONNECTED_PONG: {
				unsigned int dataLength;
				SLNet::TimeMS time;
				SLNet::BitStream stream(p->data, p->length, false);
				
				stream.IgnoreBytes(1);
				stream.Read(time);
				dataLength = p->length - sizeof(unsigned char) - sizeof(SLNet::TimeMS);
				
				printf("ID_UNCONNECTED_PONG from SystemAddress %s.\n", p->systemAddress.ToString(true));
				printf("Time is %i\n", time);
				printf("Ping is %i\n", (unsigned int) (SLNet::GetTimeMS() - time));
				printf("Data is %i bytes long.\n", dataLength);
				
				if (dataLength > 0) printf("Data is %s\n", p->data + sizeof(unsigned char) + sizeof(SLNet::TimeMS));
				
				break;
			}
		}
		
		server->DeallocatePacket(p);
	}
}

Network::~Network() {
	SLNet::RakPeerInterface::DestroyInstance(server);
}
