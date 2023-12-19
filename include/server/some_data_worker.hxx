#pragma once

#include "obj.hxx"
#include "user.hxx"
#include "server/some_data.hxx"

#include <enet/enet.h>
#include <queue>
#include <unordered_map>

namespace pnd::server
{
	class SomeDataWorker
	{
	public:
		bool operator()(const ENetEvent &event);
		void send(ENetHost *host);

		void append_player(ENetPeer *peer);
		void append_enemy(ENetPeer *peer, const User &enemy);
		void append_enemy_for_all(ENetPeer *enemy);
		void append_enemy_for_all(const User &enemy);

		void destroy_obj_for_all(const Obj &obj);
	private:
		std::unordered_map<ENetPeer *, std::queue<SomeData>> queue;
		std::queue<SomeData> broadcast_queue;
	};
} //namespace pnd::server