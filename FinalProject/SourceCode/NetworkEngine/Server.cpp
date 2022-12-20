#include "Server.h"
#include <iostream>
#include "MessageTypes.h"


void Server::ConnectToMatchmaker()
{
	m_networkThread = std::thread([this]() {
		std::cout << "Starting server\n";
		if (!NetworkBase::ConnectToMatchmaker()) {
			Fail();
			return;
		}
		if (!Annouce()) {
			Fail();
			return;
		}
		Run();
	});
}

bool Server::Alive()
{
	return !Failed();
}

bool Server::Ready()
{
	return m_ready;
}

void Server::Run()
{
	m_ready = true;
	ENetEvent event;
	while (NotJoining()) {
		while (NotJoining() && enet_host_service(m_host, &event, 0) > 0)
		{
			// handle events
			switch (event.type)
			{
			case ENET_EVENT_TYPE_RECEIVE:
				HandleRecieve(event);
				enet_packet_destroy(event.packet);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				std::cout << "Matchmaker disconnected, shutting down\n";
				Fail();
				break;
			}
		}
		if (!m_sendQueue.empty()) {
			HandleSend();
		}
	}
}

void Server::AddNewClient(ClientId a_id)
{
	std::lock_guard<std::mutex> lock(m_clientsQsMutex);
	m_newClients.push(a_id);
}

void Server::AddBadClient(ClientId a_id)
{
	std::lock_guard<std::mutex> lock(m_clientsQsMutex);
	m_badClients.push(a_id);
}

void Server::HandleRecieve(ENetEvent& a_event)
{
	MessageType msg = GetMessageType(a_event.packet);
	// check if event type is client connect
	if (msg == MessageType::ClientConnected) {
		ClientId id = *reinterpret_cast<ClientId*>(a_event.packet->data + 1);
		std::cout << "A new client connected with id " << id << "\n";
		AddNewClient(id);
		return;
	}
	// check if client disconnected
	if (msg == MessageType::ClientDisconnected) {
		ClientId id = *reinterpret_cast<ClientId*>(a_event.packet->data + 1);
		std::cout << "A client disconnected with id " << id << "\n";
		AddBadClient(id);
		return;
	}
	if (msg == MessageType::FromRelayToServer) {
		ClientResponse response(a_event.packet->data, a_event.packet->dataLength);
		std::lock_guard<std::mutex> lock(m_recieveMutex);
		m_recieveQueue.push(response);
		return;
	}
	std::cout << "UNSUPPORTED PACKAGE TYPE RECIEVED!!!!\n";
}

void Server::HandleSend()
{
	std::lock_guard<std::mutex> lock(m_sendMutex);
	while (!m_sendQueue.empty()) {
		Message msg = m_sendQueue.front().first;
		bool broadcast = m_sendQueue.front().second & 1;
		bool reliable = m_sendQueue.front().second & 2;
		m_sendQueue.pop();
		if (broadcast) {
			msg.data[0] = AsUint8(MessageType::FromServerToRelayBroadcast);
		}
		else {
			msg.data[0] = AsUint8(MessageType::FromServerToRelaySend);
		}
		ENetPacket* packet = enet_packet_create(msg.data, msg.length, reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
		enet_peer_send(m_matchmaker, 0, packet);
		msg.Clean();
	}
}



std::optional<ClientId> Server::PopNewClient()
{
	if (m_newClients.empty()) {
		return {};
	}
	std::lock_guard<std::mutex> lock(m_clientsQsMutex);
	int client = m_newClients.front();
	m_newClients.pop();
	return client;
}

std::optional<ClientId> Server::PopBadClient()
{
	if (m_badClients.empty()) {
		return {};
	}
	std::lock_guard<std::mutex> lock(m_clientsQsMutex);
	int client = m_badClients.front();
	m_badClients.pop();
	return client;
}

void Server::Broadcast(Message msg, bool reliable)
{
	std::lock_guard<std::mutex> lock(m_sendMutex);
	m_sendQueue.emplace(msg, 1 + (reliable ? 2 : 0));
}

void Server::Send(Message msg, bool reliable)
{
	std::lock_guard<std::mutex> lock(m_sendMutex);
	m_sendQueue.emplace(msg, 0 + (reliable ? 2 : 0));
}

std::optional<ClientResponse> Server::Recieve()
{
	if (m_recieveQueue.empty()) {
		return {};
	}
	std::lock_guard<std::mutex> lock(m_recieveMutex);
	ClientResponse response = m_recieveQueue.front();
	m_recieveQueue.pop();
	return response;
}

bool Server::Annouce()
{
	size_t packetSize = 1 + m_serverName.size();
	uint8_t* data = new uint8_t[packetSize];
	data[0] = static_cast<uint8_t>(MessageType::ServerAnnounce);
	memcpy(data + 1, m_serverName.c_str(), m_serverName.size());

	ENetPacket* packet = enet_packet_create(data, packetSize, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(m_matchmaker, 0, packet);
	delete[] data;
	
	//wait for responce
	ENetEvent event;
	while (enet_host_service(m_host, &event, 1000) > 0)
	{
		if (event.type == ENET_EVENT_TYPE_RECEIVE)
		{
			if (event.packet->data[0] == static_cast<uint8_t>(MessageType::ServerAnnounceOk)) {
				enet_packet_destroy(event.packet);
				std::cout << "Announcement successful\n";
				return true;
			}
			else if (event.packet->data[0] == static_cast<uint8_t>(MessageType::ServerAnnounceFail)) {
				enet_packet_destroy(event.packet);
				std::cout << "Announcement failed, the name is already taken\n";
				return false;
			}
		}
	}
	std::cout << "Announcement timed out\n";
	enet_peer_reset(m_matchmaker);
	return false;
}


