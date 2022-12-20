#pragma once

#include "NetworkBase.h"


#include <unordered_map>

class NETWORKENGINE_API Server : public IServer, public NetworkBase
{
public:
	Server() = default;

	void ConnectToMatchmaker() override;
	bool Alive() override;
	bool Ready() override;

	std::optional<ClientId> PopNewClient() override;
	std::optional<ClientId> PopBadClient() override;

	void Broadcast(Message msg, bool reliable) override;
	void Send(Message msg, bool reliable) override;

	std::optional<ClientResponse> Recieve() override;
	
	~Server() = default;
private:

	bool Annouce();
	void Run();
	void HandleRecieve(ENetEvent& a_event);
	void HandleSend();
	
	void AddNewClient(ClientId a_id);
	void AddBadClient(ClientId a_id);

	std::mutex m_clientsQsMutex;
	std::queue<ClientId> m_newClients;
	std::queue<ClientId> m_badClients;

	std::mutex m_sendMutex;
	std::queue<std::pair<Message, uint8_t>> m_sendQueue;

	std::mutex m_recieveMutex;
	std::queue<ClientResponse> m_recieveQueue;
};

