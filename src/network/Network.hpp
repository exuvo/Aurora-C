/*
 * Network.h
 *
 *  Created on: Aug 16, 2021
 *      Author: exuvo
 */

#ifndef SRC_NETWORK_NETWORK_H_
#define SRC_NETWORK_NETWORK_H_

#include "log4cxx/logger.h"

#include "slikenet/peerinterface.h"

using namespace log4cxx;

#define MAX_CONNECTIONS 32

class Network {
public:
	Network();
	~Network();
	void receive();
	
private:
	LoggerPtr log = Logger::getLogger("aurora.network");
	SLNet::RakPeerInterface* server;
};

#endif /* SRC_NETWORK_NETWORK_H_ */
