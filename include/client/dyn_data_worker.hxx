#pragma once

#include "dyn_info.hxx"

#include <queue>
#include <godot_cpp/classes/node.hpp>

typedef struct _ENetEvent ENetEvent;
typedef struct _ENetPeer ENetPeer;

namespace pnd::client
{
	class DynDataWorker
	{
	public:
		void
		operator()(const ENetEvent &event, godot::Node *client_node);
		void
		send(ENetPeer *peer);

		void
		append_info(const DynamicInfo &info);
	private:
		std::queue<DynamicInfo> queue;
	};
} //namespace pnd::client