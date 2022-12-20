#pragma once

#include "Common.h"
#include "MessageTypes.h"
#include <enet/enet.h>
#include <thread>
#include "NetworkEngine.h"
#include <mutex>
#include <queue>

constexpr auto CONNECTION_TIMEOUT = 1000;

class NETWORKENGINE_API NetworkBase {
public:
	NetworkBase();

	bool Failed() const;

	~NetworkBase();
private:
	void ParseConfig();
	bool m_failed = false;
	bool m_connected = false;
	bool m_timeToJoin = false;
protected:
	bool ConnectToMatchmaker();
	void DisconnectFromMatchmaker();
	void Fail();
	bool NotJoining();

	MessageType GetMessageType(ENetPacket* a_packet);

	uint8_t AsUint8(MessageType a_msg);

	std::string m_mmIp;
	std::string m_mmPort;
	std::string m_serverName;

	ENetHost* m_host = nullptr;
	ENetPeer* m_matchmaker = nullptr;
	bool m_ready = false;

	std::thread m_networkThread;
};


