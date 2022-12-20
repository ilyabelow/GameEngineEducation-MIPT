#include "Client.h"
#include <thread>
#include <iostream>

void Client::ConnectToMatchmaker()
{
	m_networkThread = std::thread([this]() {
		std::cout << "Starting client\n";
		if (!NetworkBase::ConnectToMatchmaker()) {
			Fail();
			return;
		}
		if (!ConnectToServer()) {
			Fail();
		}
		Run();
	});
}

bool Client::Alive()
{
	return !Failed();
}

bool Client::Ready()
{
	return m_ready;
}

void Client::Send(Message msg)
{
	std::lock_guard<std::mutex> lock(m_sendMutex);
	m_sendQueue.push(msg);
}

std::optional<Message> Client::Recieve()
{
	if (m_recieveQueue.empty()) {
		return {};
	}
	std::lock_guard<std::mutex> lock(m_recieveMutex);
	auto msg = m_recieveQueue.front();
	m_recieveQueue.pop();
	return msg;
}

bool Client::ConnectToServer()
{
	size_t packetSize = 1 + m_serverName.size();
	uint8_t* data = new uint8_t[packetSize];
	data[0] = static_cast<uint8_t>(MessageType::ServerConnect);
	memcpy(data + 1, m_serverName.c_str(), m_serverName.size());

	ENetPacket* packet = enet_packet_create(data, packetSize, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(m_matchmaker, 0, packet);
	
	// wait for responce
	ENetEvent event;
	while (enet_host_service(m_host, &event, 1000) > 0)
	{
		if (event.type == ENET_EVENT_TYPE_RECEIVE)
		{
			MessageType msg = GetMessageType(event.packet);
			if (msg == MessageType::ServerConnectOk) {
				enet_packet_destroy(event.packet);
				std::cout << "Connection to server successful" << std::endl;
				return true;
			}
			else if (msg == MessageType::ServerConnectFail) {
				enet_packet_destroy(event.packet);
				std::cout << "Connection failed, no server with such a name" << std::endl;
				return false;
			}
		}
	}
	std::cout << "Conenction timed out\n"; // should never happen
	enet_peer_reset(m_matchmaker);
	return false;
}

void Client::Run()
{
	m_ready = true;
	ENetEvent event;
	while (NotJoining()) {
		while (NotJoining() && enet_host_service(m_host, &event, 0) > 0)
		{
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

void Client::HandleSend() {
	std::lock_guard<std::mutex> lock(m_sendMutex);
	while (!m_sendQueue.empty()) {
		Message msg = m_sendQueue.front();
		m_sendQueue.pop();
		msg.data[0] = AsUint8(MessageType::FromClientToRelay);
		ENetPacket* packet = enet_packet_create(msg.data, msg.length, ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(m_matchmaker, 0, packet);
		msg.Clean();
	}
}

void Client::HandleRecieve(ENetEvent& a_event)
{
	MessageType msg = GetMessageType(a_event.packet);
	if (msg == MessageType::ServerDisconnected) {
		std::cout << "Server disconnected, shutting down\n";
		Fail();
		return;
	}
	if (msg == MessageType::FromRelayToClient) {
		Message message;
		message.length = a_event.packet->dataLength - 1;
		message.data = new uint8_t[message.length];
		memcpy(message.data, a_event.packet->data + 1, message.length);
		std::lock_guard<std::mutex> lock(m_recieveMutex);
		m_recieveQueue.push(message);
		return;
	}
	std::cout << "UNSUPPORTED PACKAGE TYPE RECIEVED!!!\n";
}
