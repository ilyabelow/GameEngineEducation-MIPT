#include "NetworkEngine.h"


#include <enet/enet.h>

#include "Server.h"
#include "Client.h"
#include <cassert>


NETWORKENGINE_API IClient* NetworkEngine::CreateClient()
{
	return new Client();
}

NETWORKENGINE_API IServer* NetworkEngine::CreateServer()
{
	return new Server();
}

NETWORKENGINE_API void NetworkEngine::Init()
{
	if (enet_initialize() != 0) {
		throw std::runtime_error("An error orrured during ENet initialization");
	}
}

NETWORKENGINE_API void NetworkEngine::Shutdown()
{
	enet_deinitialize();
}


inline void Message::Clean() {
	delete[] data;
}

inline ClientResponse::ClientResponse(uint8_t* a_packet, size_t a_packetLength) {
	id = *reinterpret_cast<ClientId*>(a_packet + 1);
	m_data = new uint8_t[a_packetLength - 1 - sizeof(ClientId)];
	memcpy(m_data, a_packet + 1 + sizeof(ClientId), a_packetLength - 1 - sizeof(ClientId));
}
