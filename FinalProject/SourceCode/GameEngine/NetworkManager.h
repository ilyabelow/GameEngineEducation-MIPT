#pragma once

#include "InputHandler.h"
#include "EntitySystem/EntitySystem.h"
#include "NetworkEngine.h"
#include "Serializer.h"
#include <unordered_set>

enum class NMState {
	Idle,
	Server,
	Client
};

struct PlayerState {
	flecs::entity e;
	InputState inputState;
};

enum class ServerAction {
	Create,
	Sync,
	Delete
};

typedef size_t EntityId;


struct ServerMessageHead {
	ServerAction action;
	EntityId id;
	Serializable::Type type;

	static ServerMessageHead CreateDelete( EntityId id) {
		ServerMessageHead head;
		head.action = ServerAction::Delete;
		head.id = id;
		return head;
	}
};

template <typename T>
struct ServerMessage {
	ServerAction action;
	EntityId id;
	Serializable::Type type = T::type;
	T data;

	static ServerMessage<T> Create(ServerAction action, EntityId id, T data) {
		ServerMessage<T> msg;
		msg.action = action;
		msg.id = id;
		msg.data = data;
		return msg;
	}
};


class NetworkManager
{
public:	
	NetworkManager(InputHandler* ih, EntitySystem* es);

	void OnFrameBegin();
	void OnFrameEnd();

	~NetworkManager();

private:
	void StartBeingServer();
	void StopBeingServer();
	void StartBeingClient();
	void StopBeingClient();

	void ClientUpdateBegin();
	void ServerUpdateBegin();
	void ClientUpdateEnd();
	void ServerUpdateEnd();

	InputHandler* m_ih;
	EntitySystem* m_es;
	NMState m_state;
	IClient* m_client = nullptr;
	IServer* m_server = nullptr;

	std::unordered_set<ClientId> m_justAdded;
	std::unordered_map<ClientId, PlayerState> m_players;
	std::unordered_map<EntityId, flecs::entity> m_entities;
};

