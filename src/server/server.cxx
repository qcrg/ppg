#include "server/server.hxx"

#include "channels.hxx"
#include "id_pull.hxx"
#include "print_utils.hxx"
#include "reg_prop.hxx"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <boost/current_function.hpp>
#include <format>

namespace pnd::server
{
	using namespace godot;

	Server::Server()
			: host{nullptr}, incoming_address{{}, 45634}, incoming_ip{"0.0.0.0"}
	{
		if (enet_initialize() != 0)
			internal::gdextension_interface_print_error_with_message(
					"Descr", "Failed to initialize ENet",
					BOOST_CURRENT_FUNCTION, __FILE__, __LINE__, false);

		set_physics_process_priority(INT32_MAX);
	}

	Server::~Server()
	{
		enet_deinitialize();
	}

	void Server::_ready()
	{
		set_physics_process(false);
		if (godot::Engine::get_singleton()->is_editor_hint())
			return;
		if (map_scene.is_null())
		{
			print(CUR_PRINT_INFO, "Map scene is not specified");
			return;
		}
		if (!map_scene->can_instantiate())
		{
			print(CUR_PRINT_INFO, "Map scene is corrupted");
			return;
		}
		map = Object::cast_to<Node3D>(map_scene->instantiate());
		map->set_name("map");
		this->add_child(map);

		if (0 != enet_address_set_host(&incoming_address,
				incoming_ip.utf8().ptr()))
			internal::gdextension_interface_print_error_with_message(
					"Descr", "An error occured while trying set host address",
					BOOST_CURRENT_FUNCTION, __FILE__, __LINE__, false);

		host = enet_host_create(&incoming_address,
				32, static_cast<int>(Channels::_count),
				0, 0);

		if (host == nullptr)
		{
			print(CUR_PRINT_INFO, "Failed to create server host."
					"Maybe port is busy: {}", incoming_address.port);
			return;
		}

		set_physics_process(true);
		get_tree()->connect("physics_frame",
				godot::Callable(this, "process"));
	}

	void Server::_physics_process(double)
	{
		// process();
	}

	void Server::process()
	{
		process_enet();

		dyn_data_worker.send(host);
		some_data_worker.send(host);
		chat_worker.send(host);
		enet_host_flush(host);
	}

	void Server::process_enet()
	{
		ENetEvent event;
		while (enet_host_service(host, &event, 0))
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
				{
					if (user_scene.is_null())
					{
						print(CUR_PRINT_INFO, "User scene is not defined");
						break;
					}
					enet_peer_timeout(event.peer,
							0, 1000, 1000);
					User *user_ptr = Object::cast_to<User>(user_scene->instantiate());
					uint8_t id = IdPull::instance().get_id();
					user_ptr->set_id(id);
					users.insert(std::make_pair(id, user_ptr));
					event.peer->data = user_ptr;
					user_ptr->set_position({0, 5, 0});
				}
				break;

				case ENET_EVENT_TYPE_RECEIVE:
					process_receive(event);
					enet_packet_destroy(event.packet);
				break;

				case ENET_EVENT_TYPE_DISCONNECT:
				{
					User *user = reinterpret_cast<User *>(event.peer->data);
					some_data_worker.destroy_obj_for_all(*user);
					users.erase(user->get_id());
					user->queue_free();
					event.peer->data = nullptr;
				}
				break;

				default:
					;
			}
		}
		process_send_dynamic_data();
	}

	void Server::process_receive(const ENetEvent &event)
	{
		switch (static_cast<Channels>(event.channelID))
		{
			case Channels::dynamic_data:
				dyn_data_worker(event);
			break;

			case Channels::some_data:
				if (some_data_worker(event))
				{
					replicate_for_user(event.peer);
					User *user = reinterpret_cast<User *>(event.peer->data);
					this->add_child(user);
					some_data_worker.append_enemy_for_all(event.peer);

					uint64_t ts = time(nullptr);
					for (const auto &[id, user] : users)
					{
						print("{}:: User [{}, ID:{}]",
								ts,
								user->get_nickname().utf8().ptr(),
								user->get_id());
					}
				}
			break;

			case Channels::chat:
				chat_worker(event);
			break;

			default:
				;
		}
	}

	void Server::process_send_dynamic_data()
	{
		for (const auto &[_, user] : users)
			dyn_data_worker.append(user);
	}

	void Server::replicate_for_user(ENetPeer *peer)
	{
		User *peer_user = reinterpret_cast<User *>(peer->data);
		some_data_worker.append_player(peer);
		for (const auto &[_, user] : users)
			if (peer_user->get_id() != user->get_id())
				some_data_worker.append_enemy(peer, *user);
	}

	void Server::set_port(int port)
	{
		if (port < 0 || port > UINT16_MAX)
		{
			print(CUR_PRINT_INFO,
					"Port must be in the range between {} and {}: port is {}",
					0, UINT16_MAX, port);
			return;
		}
		incoming_address.port = port;
	}

	int Server::get_port() const
	{
		return incoming_address.port;
	}

	void Server::set_incoming_ip(const godot::String &ip)
	{
		incoming_ip = ip;
	}

	const godot::String &Server::get_incoming_ip() const
	{
		return incoming_ip;
	}

	void Server::_exit_tree()
	{
		if (host)
		{
			enet_host_destroy(host);
			host = nullptr;
		}
		set_physics_process(false);
	}

	Ref<PackedScene> Server::get_user_scene() const
	{
		return user_scene;
	}

	godot::Ref<godot::PackedScene>
	Server::get_map_scene() const
	{
		return map_scene;
	}

	void
	Server::set_map_scene(godot::Ref<godot::PackedScene> new_map)
	{
		map_scene = new_map;
	}

	void Server::set_user_scene(Ref<PackedScene> new_user_scene)
	{
		user_scene = std::move(new_user_scene);
	}

	void Server::_bind_methods()
	{
		godot::ClassDB::bind_method(D_METHOD("set_port", "port"),
				&Server::set_port);
		godot::ClassDB::bind_method(D_METHOD("get_port"), &Server::get_port);
		ADD_PROPERTY(PropertyInfo(Variant::INT, "incoming_port"),
				"set_port", "get_port");

		godot::ClassDB::bind_method(D_METHOD("set_incoming_ip", "incoming_ip"),
				&Server::set_incoming_ip);
		godot::ClassDB::bind_method(D_METHOD("get_incoming_ip"),
				&Server::get_incoming_ip);
		ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "incoming_ip"),
				"set_incoming_ip", "get_incoming_ip");

		godot::ClassDB::bind_method(D_METHOD("set_user_scene", "user_scene"),
				&Server::set_user_scene);
		godot::ClassDB::bind_method(D_METHOD("get_user_scene"),
				&Server::get_user_scene);
		ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "user_scene",
					PROPERTY_HINT_RESOURCE_TYPE, "PackedScene"),
				"set_user_scene", "get_user_scene");
		
		REG_PROP_HINT(Server, map_scene, Variant::OBJECT,
				PROPERTY_HINT_RESOURCE_TYPE, "PackedScene");
		
		godot::ClassDB::bind_method(D_METHOD("process"), &Server::process);
	}
} // namespace pnd::server