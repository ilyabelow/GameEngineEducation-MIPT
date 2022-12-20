#pragma once

enum class MessageType {
	// Announcement messages
	ServerAnnounce,
	ServerAnnounceOk,
	ServerAnnounceFail,
	// Connecting messages
	ServerConnect,
	ServerConnectOk,
	ServerConnectFail,
	// Client status
	ClientConnected,
	ClientDisconnected,
	// Server status
	ServerDisconnected,
	// Relay
	FromServerToRelayBroadcast,
	FromServerToRelaySend,
	FromClientToRelay,
	FromRelayToClient,
	FromRelayToServer
};

