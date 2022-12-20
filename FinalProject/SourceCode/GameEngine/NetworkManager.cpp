#include "NetworkManager.h"
#include <iostream>
#include "EntitySystem/ecsControl.h"

NetworkManager::NetworkManager(InputHandler* ih, EntitySystem* es): m_ih(ih), m_state(NMState::Idle), m_es(es)
{
	NetworkEngine::Init();
}

void NetworkManager::OnFrameBegin()
{
	if (m_state == NMState::Idle)
	{
		if (m_ih->GetClickedState().test(eIC_BecomeServer))
		{
			StartBeingServer();
		} else
		if (m_ih->GetClickedState().test(eIC_BecomeClient))
		{
			StartBeingClient();
		}
		return;
	}

	if (m_state == NMState::Server)
	{
		if (m_ih->GetClickedState().test(eIC_BecomeServer) || !m_server->Alive())
		{
			StopBeingServer();
			return;
		}
		if (m_server->Ready())
		{
			ServerUpdateBegin();
		}
	}

	if (m_state == NMState::Client)
	{
		if (m_ih->GetClickedState().test(eIC_BecomeClient) || !m_client->Alive()) 
		{
			StopBeingClient();
			return;
		}
		if (m_client->Ready())
		{
			ClientUpdateBegin();
		}
	}
}

void NetworkManager::OnFrameEnd()
{
	if (m_state == NMState::Idle)
	{
		// Do nothing I guess
	}
	if (m_state == NMState::Server)
	{
		if (m_server->Ready())
		{
			ServerUpdateEnd();
		}
	}
	if (m_state == NMState::Client)
	{
		if (m_client->Ready())
		{
			ClientUpdateEnd();
		}
	}
}

NetworkManager::~NetworkManager()
{
	if (m_client != nullptr)
	{
		delete m_client;
	}
	if (m_server != nullptr)
	{
		delete m_server;
	}
	NetworkEngine::Shutdown();
}

//
// ================================================================================== SERVER
//

void NetworkManager::StartBeingServer()
{
	std::cout << "==================================== Becoming server" << std::endl;
	m_es->StartSync();
	m_state = NMState::Server;
	m_server = NetworkEngine::CreateServer();
	m_server->ConnectToMatchmaker();
}

void NetworkManager::StopBeingServer()
{
	std::cout << "Stopping server" << std::endl;
	m_es->StopSync();
	m_state = NMState::Idle;
	delete m_server;
	m_server = nullptr;
	// clear players
	for (auto& player: m_players)
	{
		player.second.e.add<Destruct>();
	}
	m_players.clear();
}

void NetworkManager::ServerUpdateBegin()
{
	// Connecting new clients
	auto id_wrapped = m_server->PopNewClient();
	while (id_wrapped)
	{
		ClientId id = id_wrapped.value();
		std::cout << "New player created: " << id << std::endl;

		// SYNC EVERYTHING TO A NEW PLAYER
		m_es->ecs.query<Sync>().each([this, id](flecs::entity e, Sync& s)
			{
				if (s.type == Serializable::Type::Player)
				{
					auto msg = ServerMessage<Serializable::Player>::Create(ServerAction::Create, e.id(), Serializable::Player::Serialize(e));
					m_server->Send(Message::CreateSend(id, msg), true);
				}
				if (s.type == Serializable::Type::Enemy)
				{
					auto msg = ServerMessage<Serializable::Enemy>::Create(ServerAction::Create, e.id(), Serializable::Enemy::Serialize(e));
					m_server->Send(Message::CreateSend(id, msg), true);
				}
				if (s.type == Serializable::Type::Bullet)
				{
					auto msg = ServerMessage<Serializable::Bullet>::Create(ServerAction::Create, e.id(), Serializable::Bullet::Serialize(e));
					m_server->Send(Message::CreateSend(id, msg), true);
				}
			}
		);
		// Creating new player
		m_justAdded.clear();
		m_players[id] = PlayerState();
		m_players[id].e = m_es->CreatePlayer(&m_players[id].inputState);
		m_justAdded.insert(id);
		id_wrapped = m_server->PopNewClient();
	}
	
	// Removing disconnected clients
	 id_wrapped = m_server->PopBadClient();
	while (id_wrapped)
	{
		ClientId id = id_wrapped.value();
		std::cout << "Player removed: " << id << std::endl;
		m_players[id].e.add<Destruct>();
		m_players.erase(id);
		id_wrapped = m_server->PopBadClient();
	}
	
	// Get player states
	auto msg_wrapped = m_server->Recieve();
	while (msg_wrapped)
	{
		ClientResponse& msg = msg_wrapped.value();
		m_players[msg.id].inputState = msg.GetData<InputState>();
		msg_wrapped = m_server->Recieve();
	}
}

void NetworkManager::ServerUpdateEnd()
{
	// Broadcast delete
	for (auto& e : m_es->m_toSyncedDelete) {
		auto msg = ServerMessageHead::CreateDelete(e.id());
		m_server->Broadcast(Message::Create(msg), true);
	}
	m_es->m_toSyncedDelete.clear();
	
	// Broadcast create
	for (flecs::entity e : m_es->m_toSyncedCreate) {
		if (!e.is_valid()) {
			continue;
		}
		auto s = e.get<Sync>();

		if (s->type == Serializable::Type::Player)
		{
			for (auto& id_state : m_players) {
				auto id = id_state.first;
				if (!m_justAdded.contains(id)) {
					auto msg = ServerMessage<Serializable::Player>::Create(ServerAction::Create, e.id(), Serializable::Player::Serialize(e));
					m_server->Send(Message::CreateSend(id, msg), false);
				}
			}
		}
		if (s->type == Serializable::Type::Enemy)
		{
			auto msg = ServerMessage<Serializable::Enemy>::Create(ServerAction::Create, e.id(), Serializable::Enemy::Serialize(e));
			m_server->Broadcast(Message::Create(msg), true);
		}
		if (s->type == Serializable::Type::Bullet)
		{
			auto msg = ServerMessage<Serializable::Bullet>::Create(ServerAction::Create, e.id(), Serializable::Bullet::Serialize(e));
			m_server->Broadcast(Message::Create(msg), true);
		}
	}
	m_es->m_toSyncedCreate.clear();

	// Broadcast update
	for (flecs::entity e : m_es->m_toSync) {
		if (!e.is_valid()) {
			continue;
		}
		auto s = e.get<Sync>();

		if (s->type == Serializable::Type::Player)
		{
			auto msg = ServerMessage<Serializable::Player>::Create(ServerAction::Sync, e.id(), Serializable::Player::Serialize(e));
			m_server->Broadcast(Message::Create(msg), false);
		}
		if (s->type == Serializable::Type::Enemy)
		{
			auto msg = ServerMessage<Serializable::Enemy>::Create(ServerAction::Sync, e.id(), Serializable::Enemy::Serialize(e));
			m_server->Broadcast(Message::Create(msg), false);
		}
		if (s->type == Serializable::Type::Bullet)
		{
			auto msg = ServerMessage<Serializable::Bullet>::Create(ServerAction::Sync, e.id(), Serializable::Bullet::Serialize(e));
			m_server->Broadcast(Message::Create(msg), false);
		}
	}
	m_es->m_toSync.clear();
}

//
// =========================================================================================== CLIENT
//

void NetworkManager::StartBeingClient()
{
	std::cout << "==================================== Becoming client" << std::endl;
	m_es->HaltLocalCreation();
	m_es->player.set<Position>({0, 0, -3});
	m_state = NMState::Client;
	m_client = NetworkEngine::CreateClient();
	m_client->ConnectToMatchmaker();
}

void NetworkManager::StopBeingClient()
{
	std::cout << "Stopping client" << std::endl;
	m_es->ResumeLocalCreation();
	m_state = NMState::Idle;
	for (auto& e : m_entities) {
		if (e.second.is_valid()) {
			e.second.add<Destruct>().remove<Remote>();
		}
	}
	m_entities.clear();
	delete m_client;
	m_client = nullptr;
	m_es->StartTargets();
}


void NetworkManager::ClientUpdateBegin()
{
	m_client->Send(Message::Create(m_ih->GetInputState()));

	auto msg_wrapped = m_client->Recieve();
	while (msg_wrapped)
	{
		Message& msg = msg_wrapped.value();
		ServerMessageHead head = msg.GetData<ServerMessageHead>(false);
		if (head.type == Serializable::Type::Player) {
			ServerMessage<Serializable::Player> data = msg.GetData<ServerMessage<Serializable::Player>>(true);
			if (data.action == ServerAction::Create) {
				auto e = m_es->CreatePlayer();
				m_entities[data.id] = e;
				data.data.Apply(e);
			}
			else if (data.action == ServerAction::Sync) {
				if (m_entities[data.id].is_valid()) {
					data.data.Apply(m_entities[data.id]);
				}
			}
		}
		if (head.type == Serializable::Type::Bullet) {
			ServerMessage<Serializable::Bullet> data = msg.GetData<ServerMessage<Serializable::Bullet>>(true);
			if (data.action == ServerAction::Create) {
				auto e = m_es->CreateBullet();
				m_entities[data.id] = e;
				data.data.Apply(e);
			}
			else if (data.action == ServerAction::Sync) {
				if (m_entities[data.id].is_valid()) {
					data.data.Apply(m_entities[data.id]);
				}
			}
		}
		if (head.type == Serializable::Type::Enemy) {
			ServerMessage<Serializable::Enemy> data = msg.GetData<ServerMessage<Serializable::Enemy>>(true);
			if (data.action == ServerAction::Create) {
				auto e = m_es->CreateTarget();
				m_entities[data.id] = e;
				data.data.Apply(e);
			}
			else if (data.action == ServerAction::Sync) {
				if (m_entities[data.id].is_valid()) {
					data.data.Apply(m_entities[data.id]);
				}
			}
		}
		if (head.action == ServerAction::Delete) {
			if (m_entities[head.id].is_valid()) {
				m_entities[head.id].add<Destruct>();
				m_entities[head.id].remove<Remote>();
			}
			m_entities.erase(head.id);

		}
		msg_wrapped = m_client->Recieve();
	}
}



void NetworkManager::ClientUpdateEnd() {
	// TODO sync from server here?
}
