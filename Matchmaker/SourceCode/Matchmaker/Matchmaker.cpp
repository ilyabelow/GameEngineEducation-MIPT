#include "Matchmaker.h"
#include <iostream>

Matchmaker::Matchmaker()
{
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = 8888;

	m_server = enet_host_create(&address, 32, 2, 0, 0);
	if (m_server == NULL)
	{
		std::cerr << "An error occurred while initializing server, exiting\n";
		exit(EXIT_FAILURE);
	}
}

void Matchmaker::Run()
{
	ENetEvent event;
	while (true)
	{
		while (enet_host_service(m_server, &event, 0) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				std::cout << "A new client connected from " << event.peer->address.host << ":" << event.peer->address.port << "\n";
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				HandleRecieve(event);
				enet_packet_destroy(event.packet);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				std::cout << event.peer->address.host << ":" << event.peer->address.port << " disconnected\n";
				HandleDisconnect(event);
				break;
			default:
				std::cout << "UNKNOWN EVENT TYPE RECIEVED!\n";
			}
		}
	}
}

inline MessageType Matchmaker::GetMessageType(ENetPacket* a_packet)
{
	return static_cast<MessageType>(a_packet->data[0]);
}

inline uint8_t Matchmaker::AsUint8(MessageType a_msg)
{
	return static_cast<uint8_t>(a_msg);
}



void Matchmaker::HandleDisconnect(ENetEvent& a_event)
{
	// if server disconnected
	if (m_serverToName.contains(a_event.peer)) {
		std::string name = m_serverToName[a_event.peer];
		std::cout << "Server "<< a_event.peer->address.host << ':' << a_event.peer->address.port << " named " << name << " disconnected\n";
		m_nameToServer.erase(name);
		m_serverToName.erase(a_event.peer);
		for (ENetPeer* client : m_serverToClients[a_event.peer]) {
			MessageType msg = MessageType::ServerDisconnected;
			ENetPacket* packet = enet_packet_create(&msg, sizeof(MessageType), ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(client, 0, packet);
			
			m_clientToServer.erase(client);
			m_clientToClientId.erase(client);
		}
		m_serverToClients.erase(a_event.peer);
		m_serverToClientId.erase(a_event.peer);
		return;
	}

	// if client disconnected
	if (m_clientToServer.contains(a_event.peer)) {
		ENetPeer* server = m_clientToServer[a_event.peer];
		m_clientToServer.erase(a_event.peer);
		ClientId id = m_clientToClientId[a_event.peer];
		m_clientToClientId.erase(a_event.peer);
		m_serverToClients[server].erase(a_event.peer);
		
		std::cout << "Client " << a_event.peer->address.host << ":" << a_event.peer->address.port << " disconnected from server named " << m_serverToName[server] <<"\n";
		
		uint8_t* data = new uint8_t[sizeof(MessageType) + sizeof(ClientId)];
		data[0] = AsUint8(MessageType::ClientDisconnected);
		memcpy(data + 1, &id, sizeof(ClientId));
		ENetPacket* packet = enet_packet_create(data, sizeof(MessageType) + sizeof(ClientId), ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(server, 0, packet);
		delete[] data;
		return;
	}
}

void Matchmaker::HandleRecieve(ENetEvent& a_event)
{
	MessageType msg = GetMessageType(a_event.packet);
	if (msg == MessageType::FromClientToRelay || msg == MessageType::FromServerToRelayBroadcast || msg == MessageType::FromServerToRelaySend)
	{
		HandleRelay(a_event.peer, a_event.packet, msg);
	}
	if (msg == MessageType::ServerAnnounce)
	{
		HandleAnnounce(a_event.peer, NameFromConnect(a_event.packet));
	}
	else if (msg == MessageType::ServerConnect)
	{
		HandleConnect(a_event.peer, NameFromConnect(a_event.packet));
	}
}

std::string Matchmaker::NameFromConnect(ENetPacket* a_packet)
{
	return { reinterpret_cast<char*>(a_packet->data + 1), a_packet->dataLength - 1 };
}

void Matchmaker::HandleAnnounce(ENetPeer* a_peer, std::string a_name)
{
	if (m_nameToServer.contains(a_name))
	{
		std::cout << "Server " << a_peer->address.host << ":" << a_peer->address.port << " tried to announce name " << a_name << ", but it already exists\n";
		MessageType msg = MessageType::ServerAnnounceFail;
		ENetPacket* packet = enet_packet_create(&msg, 1, ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(a_peer, 0, packet);
	}
	else
	{
		std::cout << "Server " << a_peer->address.host << ":" << a_peer->address.port << " successfully announced name " << a_name << "\n";
		m_nameToServer[a_name] = a_peer;
		m_serverToName[a_peer] = a_name;
		m_serverToClients[a_peer] = {};
		m_serverToClientId[a_peer] = 0;
		MessageType msg = MessageType::ServerAnnounceOk;
		ENetPacket* packet = enet_packet_create(&msg, 1, ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(a_peer, 0, packet);
	}
}

void Matchmaker::HandleConnect(ENetPeer* a_peer, std::string a_name)
{
	if (m_nameToServer.contains(a_name))
	{
		std::cout << "Client " << a_peer->address.host << ":" << a_peer->address.port << " connected to server " << a_name << "\n";
		{ // Send OK to client
			MessageType msg = MessageType::ServerConnectOk;
			ENetPacket* packet = enet_packet_create(&msg, sizeof(MessageType), ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(a_peer, 0, packet);
		}
		{ // notify server about connection
			auto server = m_nameToServer[a_name];
			
			uint8_t* data = new uint8_t[sizeof(MessageType) + sizeof(uint32_t)];
			data[0] = AsUint8(MessageType::ClientConnected);
			ClientId id = m_serverToClientId[server];
			m_serverToClientId[server]++;
			memcpy(data + 1, &id, sizeof(ClientId));
			
			ENetPacket* packet = enet_packet_create(data, sizeof(MessageType)+sizeof(ClientId), ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(server, 0, packet);
			delete[] data;

			m_serverToClients[server].insert(a_peer);
			m_clientToServer[a_peer] = server;
			m_clientToClientId[a_peer] = id;
		}
	}
	else
	{
		std::cout << "Client " << a_peer->address.host << ":" << a_peer->address.port << " tried connecting to " << a_name << " but this server does not exist.\n";
		MessageType msg = MessageType::ServerConnectFail;
		ENetPacket* packet = enet_packet_create(&msg, sizeof(MessageType), ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(a_peer, 0, packet);		
	}
}

void Matchmaker::HandleRelay(ENetPeer* a_peer, ENetPacket* a_packet, const MessageType a_type)
{
	if (a_type == MessageType::FromClientToRelay) {
		ENetPeer* client = a_peer;
		if (!m_clientToServer.contains(client)) {
			return;
		}
		ENetPeer* server = m_clientToServer[client];

		
		ClientId id = m_clientToClientId[client];
		uint8_t* data = new uint8_t[a_packet->dataLength + sizeof(ClientId)];

		data[0] = AsUint8(MessageType::FromRelayToServer);
		memcpy(data + 1, &id, sizeof(ClientId));
		memcpy(data + sizeof(ClientId) + 1, a_packet->data + 1, a_packet->dataLength - 1);
		ENetPacket* packet = enet_packet_create(data, a_packet->dataLength + sizeof(ClientId), 0);
		enet_peer_send(server, 0, packet);
		delete[] data;
	}
	bool shouldBeReliable = a_packet->flags & ENET_PACKET_FLAG_RELIABLE;
	ENetPeer* server = a_peer;
	if (a_type == MessageType::FromServerToRelayBroadcast) {

		uint8_t* data = new uint8_t[a_packet->dataLength];
		data[0] = AsUint8(MessageType::FromRelayToClient);
		memcpy(data + 1, a_packet->data + 1, a_packet->dataLength - 1);
		ENetPacket* packet = enet_packet_create(data, a_packet->dataLength, shouldBeReliable ? ENET_PACKET_FLAG_RELIABLE : 0);
		for (ENetPeer* client : m_serverToClients[server]) {
			enet_peer_send(client, 0, packet);
		}
		delete[] data;
	}
	if (a_type == MessageType::FromServerToRelaySend) {
		size_t outgoingPacketSize = a_packet->dataLength - sizeof(ClientId);
		uint8_t* data = new uint8_t[outgoingPacketSize];
		data[0] = AsUint8(MessageType::FromRelayToClient);
		ClientId id = *reinterpret_cast<ClientId*>(a_packet->data + 1);
		
		memcpy(data + 1, a_packet->data + 1 + sizeof(ClientId), outgoingPacketSize);
		ENetPacket* packet = enet_packet_create(data, outgoingPacketSize, shouldBeReliable ? ENET_PACKET_FLAG_RELIABLE : 0);
		for (ENetPeer* client : m_serverToClients[server]) {
			if (m_clientToClientId[client] == id) {
				// tmp solution
				enet_peer_send(client, 0, packet);
				break;
			}
		}
		delete[] data;
	}
}
