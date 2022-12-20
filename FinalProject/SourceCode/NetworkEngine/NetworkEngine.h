#pragma once

#include "Common.h"
#include <optional>

typedef uint32_t ClientId;



struct NETWORKENGINE_API Message {
	Message() = default;
	
	void Clean();

	uint8_t* data;
	size_t length;
	
	template <typename T>
	T GetData(bool a_final) {
		T t;
		memcpy(&t, data, sizeof(T));
		if (a_final) {
			Clean();
		}
		return t;
	}

	template <typename T>
	static Message Create(T a_data);

	template <typename T>
	static Message CreateSend(ClientId id, T a_data);
};


struct NETWORKENGINE_API ClientResponse {

	ClientResponse(uint8_t* a_packet, size_t a_packetLength);
	template <typename T>
	T GetData() {
		T t;
		memcpy(&t, m_data, sizeof(T));
		delete[] m_data;
		return t;
	}
	ClientId id;

private:
	uint8_t* m_data;
};

class NETWORKENGINE_API IClient {
public:
	virtual void ConnectToMatchmaker() = 0;
	virtual bool Alive() = 0;
	virtual bool Ready() = 0;
	virtual void Send(Message a_message) = 0;
	virtual std::optional<Message> Recieve() = 0;
	virtual ~IClient() = default;
};

class NETWORKENGINE_API IServer {
public:
	virtual void ConnectToMatchmaker() = 0;
	virtual bool Alive() = 0;
	virtual bool Ready() = 0;
	virtual std::optional<ClientId> PopNewClient() = 0;
	virtual std::optional<ClientId> PopBadClient() = 0;
	virtual void Broadcast(Message msg, bool reliable) = 0;
	virtual void Send(Message msg, bool reliable) = 0;
	virtual std::optional<ClientResponse> Recieve() = 0;
	virtual ~IServer() = default;
};


namespace NetworkEngine {
	NETWORKENGINE_API IClient* CreateClient();
	NETWORKENGINE_API IServer* CreateServer();
	NETWORKENGINE_API void Init();
	NETWORKENGINE_API void Shutdown();
}

template<typename T>
inline Message Message::Create(T a_data) {
	Message msg = {};
	msg.length = sizeof(T) + 1;
	msg.data = new uint8_t[msg.length];
	memcpy(msg.data+1, &a_data, sizeof(T));
	return msg;
}

template<typename T>
inline Message Message::CreateSend(ClientId id, T a_data)
{
	Message msg = {};
	msg.length = sizeof(T) + sizeof(ClientId) + 1;
	msg.data = new uint8_t[msg.length];
	memcpy(msg.data + 1, &id, sizeof(ClientId));
	memcpy(msg.data + sizeof(ClientId) + 1, &a_data, sizeof(T));
	return msg;
}
