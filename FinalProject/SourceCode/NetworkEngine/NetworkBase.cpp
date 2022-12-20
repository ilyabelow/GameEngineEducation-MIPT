#include "NetworkBase.h"

#include <sstream>
#include <fstream>
#include <iostream>

NetworkBase::NetworkBase()
{
	m_host = enet_host_create(NULL, 1, 2, 0, 0);
	// SHOULD NEVER FAIL, TODO REMOVE
	if (m_host == NULL) {
		throw std::runtime_error("An error orrured during ENet host creation");
	}
	ParseConfig();
}

bool NetworkBase::Failed() const
{
	return m_failed;
}

NetworkBase::~NetworkBase()
{
	m_timeToJoin = true;
	m_networkThread.join();
	if (m_connected) {
		DisconnectFromMatchmaker();
	}
	enet_host_destroy(m_host);
}

void NetworkBase::ParseConfig()
{
	std::fstream file("../../../Assets/Configs/network.conf");
	std::string line_string;
	while (std::getline(file, line_string))
	{
		std::istringstream line_stream(line_string);
		std::string key, value;
		std::getline(line_stream, key, '=');
		std::getline(line_stream, value);
		// TODO error handling if config has no value?
		if (key == "matchmaker_ip") {
			m_mmIp = value;
		}
		else if (key == "matchmaker_port") {
			m_mmPort = value;
		}
		else if (key == "server_name") {
			m_serverName = value;
		}
	}
	// spacer
	std::cout << "----------------------" << std::endl;
	std::cout << "NetworkEngine config:" << std::endl;
	std::cout << "Matchmaker IP: " << m_mmIp << std::endl;
	std::cout << "Matchmaker Port: " << m_mmPort << std::endl;
	std::cout << "Server Name: " << m_serverName << std::endl;
	std::cout << "----------------------" << std::endl;
	std::cout << std::endl;
}

bool NetworkBase::ConnectToMatchmaker()
{
	// connect to matchmaker
	ENetAddress address;
	enet_address_set_host_ip(&address, m_mmIp.c_str());
	address.port = std::stoi(m_mmPort);
	m_matchmaker = enet_host_connect(m_host, &address, 2, 0);
	// SHOULD NEVER FAIL, TODO REMOVE
	if (m_matchmaker == NULL) {
		throw std::runtime_error("No available peers for initiating an ENet connection");
	}
	ENetEvent event;
	// TODO async connection
	if (enet_host_service(m_host, &event, CONNECTION_TIMEOUT) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
	{
		std::cout << "Connection to matchmaker successful" << std::endl;
		m_connected = true;
		return true;
	}
	else 
	{
		enet_peer_reset(m_matchmaker);
		std::cout << "Connection to matchmaker failed, are you sure it's running?" << std::endl;
		return false;
	}
}

void NetworkBase::DisconnectFromMatchmaker()
{
	enet_peer_disconnect(m_matchmaker, 0);
	ENetEvent event;
	while (enet_host_service(m_host, &event, 100) > 0) {
		switch (event.type) {
		case ENET_EVENT_TYPE_RECEIVE:
			enet_packet_destroy(event.packet);
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			std::cout << "Disconnected from matchmaker" << std::endl;
			return;
		}
	}
}

void NetworkBase::Fail()
{
	m_ready = false;
	m_failed = true;
	if (m_connected) {
		DisconnectFromMatchmaker();
	}
}

bool NetworkBase::NotJoining()
{
	return !m_timeToJoin;
}

inline MessageType NetworkBase::GetMessageType(ENetPacket* a_packet)
{
	return static_cast<MessageType>(a_packet->data[0]);
}

inline uint8_t NetworkBase::AsUint8(MessageType a_msg)
{
	return static_cast<uint8_t>(a_msg);
}
