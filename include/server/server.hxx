#pragma once

#include "user.hxx"
#include "chat_worker.hxx"
#include "some_data_worker.hxx"
#include "dynamic_data_worker.hxx"

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <boost/noncopyable.hpp>
#include <enet/enet.h>
#include <unordered_map>

namespace pnd::server
{
	class Server : public godot::Node, public boost::noncopyable
	{
		GDCLASS(Server, Node)
	public:
		Server();
		virtual ~Server();
		virtual void _ready() override;
		virtual void _exit_tree() override;
		virtual void _physics_process(double delta) override;
		void process();

		void set_port(int port);
		void set_incoming_ip(const godot::String &ip);
		int get_port() const;
		const godot::String &get_incoming_ip() const;

		godot::Ref<godot::PackedScene> get_user_scene() const;
		void set_user_scene(godot::Ref<godot::PackedScene> scene);
		godot::Ref<godot::PackedScene>
		get_map_scene() const;
		void
		set_map_scene(godot::Ref<godot::PackedScene> map);
	protected:
		static void _bind_methods();
	private:
		ENetHost *host;
		ENetAddress incoming_address;
		godot::String incoming_ip;
		godot::Ref<godot::PackedScene> user_scene;
		godot::Ref<godot::PackedScene> map_scene;
		godot::Node3D *map;
		std::unordered_map<uint8_t, User *> users;

		DynamicDataWorker dyn_data_worker;
		SomeDataWorker some_data_worker;
		ChatWorker chat_worker;

		void process_enet();
		void process_receive(const ENetEvent &event);
		void process_send_dynamic_data();
		void replicate_for_user(ENetPeer *peer);
	};
} //namespace pnd::server