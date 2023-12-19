#pragma once

#include "some_data_worker.hxx"
#include "dyn_data_worker.hxx"

#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/node.hpp>
#include <unordered_map>

typedef struct _ENetHost ENetHost;
typedef struct _ENetPeer ENetPeer;
typedef struct _ENetEvent ENetEvent;

//FIXME
struct ChatWorker {
	void operator()(const ENetEvent &) {}
	void send(ENetPeer *) {}
};

namespace pnd
{
	struct User;
} //namespace pnd

namespace pnd::client
{
	class Client : public godot::Node
	{
		GDCLASS(Client, Node)
	public:
		Client();
		virtual ~Client();

		virtual void
		_ready() override;
		virtual void
		_exit_tree() override;
		virtual void
		_physics_process(double delta) override;

		void
		set_port(int port);
		int
		get_port() const;

		void
		set_ip(godot::String ip);
		godot::String
		get_ip() const;

		void
		set_player_scene(godot::Ref<godot::PackedScene> player);
		godot::Ref<godot::PackedScene>
		get_player_scene() const;
		void
		set_enemy_scene(godot::Ref<godot::PackedScene> enemy);
		godot::Ref<godot::PackedScene>
		get_enemy_scene() const;
	protected:
		static void _bind_methods();
	private:
		User *player;
		ENetHost *client;
		ENetPeer *peer;
		godot::String ip;
		int port;
		godot::Ref<godot::PackedScene> player_scene;
		godot::Ref<godot::PackedScene> enemy_scene;
		std::unordered_map<uint8_t, User *> enemies;

		DynDataWorker dyn_data_worker;
		SomeDataWorker some_data_worker;
		ChatWorker chat_worker;

		void
		process_enet();
		void
		process_enet_receive(const ENetEvent &event);
	};
} //namespace pnd::client