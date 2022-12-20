#pragma once

#include "NetworkEngine.h"
#include "NetworkBase.h"

class NETWORKENGINE_API Client : public IClient, public NetworkBase 
{
public:
	void ConnectToMatchmaker() override;
	bool Alive() override;
	bool Ready() override;

	void Send(Message msg) override;
	std::optional<Message> Recieve() override;
private:
	bool ConnectToServer();
	void Run();
	void HandleRecieve(ENetEvent& a_event);
	void HandleSend();

	std::mutex m_sendMutex;
	std::queue<Message> m_sendQueue;
	std::mutex m_recieveMutex;
	std::queue<Message> m_recieveQueue;
};