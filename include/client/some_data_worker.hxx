#pragma once

#include "capnp/connection.capnp.h"
#include "capnp/server.capnp.h"
#include "server/some_data.hxx"

#include <queue>
#include <string>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/packed_scene.hpp>

typedef struct _ENetPeer ENetPeer;
typedef struct _ENetEvent ENetEvent;

namespace pnd
{
	struct User;
} //namespace pnd

namespace pnd::client
{
	struct SomeData
	{
		connection::Action::Data::Which type;
		std::string nickname;
	};

	class SomeDataWorker
	{
	public:
		void operator()(const ENetEvent &event, godot::Node *client_node);
		void send(ENetPeer *peer);

		void append_hello(std::string nickname);

		godot::Ref<godot::PackedScene> player_scene;
		godot::Ref<godot::PackedScene> enemy_scene;
	private:
		std::queue<SomeData> queue;

		void create_object(const server::SomeData &data, godot::Node *client_node);
		void delete_object(int id, godot::Node *client_node);
	};
} //namespace pnd::client