#pragma once
#include <enet/enet.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "MessageTypes.h"

typedef uint32_t ClientId;


class Matchmaker
{
public:
	Matchmaker();

	void Run();
private:

	MessageType GetMessageType(ENetPacket* a_packet);

	uint8_t AsUint8(MessageType a_msg);


	
	void HandleDisconnect(ENetEvent& a_event);

	void HandleRecieve(ENetEvent& a_event);
	std::string NameFromConnect(ENetPacket* a_packet);

	void HandleAnnounce(ENetPeer* a_peer, std::string a_name);
	void HandleConnect(ENetPeer* a_peer, std::string a_name);
	void HandleRelay(ENetPeer* a_client, ENetPacket* a_packet, const MessageType a_type);

	ENetHost* m_server;

	std::unordered_map<ENetPeer*, ENetPeer*> m_clientToServer; // which server is connected to a client
	std::unordered_map<ENetPeer*, ClientId> m_clientToClientId; // what id a client has
	
	std::unordered_map<ENetPeer*, std::unordered_set< ENetPeer*>> m_serverToClients; // which clients are connected to a server
	std::unordered_map<ENetPeer*, ClientId> m_serverToClientId; // current id for a new client for a server
	
	std::unordered_map<std::string, ENetPeer*> m_nameToServer;
	std::unordered_map<ENetPeer*, std::string> m_serverToName;
};


