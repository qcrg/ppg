#include "client/client.hxx"

#include "print_utils.hxx"
#include "channels.hxx"
#include "print_utils.hxx"
#include "reg_prop.hxx"
#include "user.hxx"

#include <enet/enet.h>
#include <godot_cpp/classes/engine.hpp>

namespace pnd::client
{
	Client::Client()
		: player{nullptr}
		, client{nullptr}
		, peer{nullptr}
		, ip{"127.0.0.1"}
		, port{45634}
	{
		if (enet_initialize() != 0)
			print(CUR_PRINT_INFO, "Failed to initialize ENet");
		set_physics_process(INT32_MAX);
	}

	Client::~Client()
	{
		_exit_tree();
		enet_deinitialize();
	}

	void
	Client::_ready()
	{
		set_physics_process(false);
		if (godot::Engine::get_singleton()->is_editor_hint())
			return;
		
		ENetAddress addr;
		if (0 != enet_address_set_host(&addr, ip.utf8().ptr()))
		{
			print(CUR_PRINT_INFO, "IP not resolved: {}", ip.utf8().ptr());
			return;
		}
		addr.port = port;

		client = enet_host_create(nullptr,
				1, static_cast<int>(Channels::_count),
				0, 0);
		if (client == nullptr)
		{
			print(CUR_PRINT_INFO, "Failed to create ENet client HOST");
			return;
		}

		peer = enet_host_connect(client, &addr,
				static_cast<int>(Channels::_count),
				0);
		if (peer == nullptr)
		{
			print(CUR_PRINT_INFO, "Failed to connect to the address: {}:{}",
					ip.utf8().ptr(), port);
			this->~Client();
			return;
		}
		set_physics_process(true);
	}

	void
	Client::_exit_tree()
	{
		if (peer)
		{
			enet_peer_disconnect(peer, 0);
			enet_host_flush(client);
			peer = nullptr;
		}
		if (client)
		{
			enet_host_destroy(client);
			client = nullptr;
		}
		set_physics_process(false);
	}

	void
	Client::_physics_process(double)
	{
		if (peer == nullptr)
			return;
		process_enet();
		if (player == nullptr)
			player = Object::cast_to<User>(this->find_child("player*", false, false));
		else
			dyn_data_worker.append_info(*player->get_dyn_info());
		dyn_data_worker.send(peer);
		some_data_worker.send(peer);
		chat_worker.send(peer);
		enet_host_flush(client);
	}

	void
	Client::set_port(int port)
	{
		if (port < 0 || port > UINT16_MAX)
		{
			print(CUR_PRINT_INFO,
					"Port must be in the range between {} and {}: port is {}",
					0, UINT16_MAX, port);
			return;
		}
		this->port = port;
	}

	int
	Client::get_port() const
	{
		return port;
	}

	void
	Client::set_ip(godot::String ip)
	{
		this->ip = ip;
	}

	godot::String
	Client::get_ip() const
	{
		return ip;
	}

	void
	Client::set_player_scene(godot::Ref<godot::PackedScene> player)
	{
		player_scene = player;
		some_data_worker.player_scene = player;
	}

	godot::Ref<godot::PackedScene>
	Client::get_player_scene() const
	{
		return player_scene;
	}

	void
	Client::process_enet()
	{
		ENetEvent event;
		while (enet_host_service(client, &event, 0))
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
					some_data_worker.append_hello("shlep");
				break;

				case ENET_EVENT_TYPE_RECEIVE:
					process_enet_receive(event);
					enet_packet_destroy(event.packet);
				break;

				case ENET_EVENT_TYPE_DISCONNECT:
				{
					//remove all object
				}
				break;

				default:
					;
			}
		}
	}

	void
	Client::process_enet_receive(const ENetEvent &event)
	{
		switch (static_cast<Channels>(event.channelID))
		{
			case Channels::dynamic_data:
				dyn_data_worker(event, this);
			break;

			case Channels::some_data:
				some_data_worker(event, this);
			break;

			case Channels::chat:
				chat_worker(event);
			break;
			
			default:
				;
		}
	}

	void
	Client::set_enemy_scene(godot::Ref<godot::PackedScene> enemy)
	{
		enemy_scene = enemy;
		some_data_worker.enemy_scene = enemy;
	}

	godot::Ref<godot::PackedScene>
	Client::get_enemy_scene() const
	{
		return enemy_scene;
	}

	void
	Client::_bind_methods()
	{
		using namespace godot;
		REG_PROP(Client, ip, Variant::STRING);
		REG_PROP(Client, port, Variant::INT);
		REG_PROP_HINT(Client, player_scene, Variant::OBJECT,
				PROPERTY_HINT_RESOURCE_TYPE, "PackedScene");
		REG_PROP_HINT(Client, enemy_scene, Variant::OBJECT,
				PROPERTY_HINT_RESOURCE_TYPE, "PackedScene");
	}
} //namespace pnd::client