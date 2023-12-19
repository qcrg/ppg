#include "client/some_data_worker.hxx"

#include "capnp/server.capnp.h"
#include "capnp/client.capnp.h"
#include "user.hxx"
#include "print_utils.hxx"
#include "id_pull.hxx"

#include <capnp/message.h>
#include <enet/enet.h>
#include <cstring>

using namespace connection;
using namespace godot;

namespace pnd::client
{
	void SomeDataWorker::operator()(const ENetEvent &event, godot::Node *client)
	{
		kj::ArrayPtr<const capnp::word> segment(
			reinterpret_cast<const capnp::word *>(event.packet->data),
			event.packet->dataLength / sizeof(capnp::word)
		);
		capnp::SegmentArrayMessageReader root({&segment, 1});
		::server::ObjPacket::Reader builder = root.getRoot<::server::ObjPacket>();
		auto list = builder.getList();
		for (uint i = 0; i < list.size(); i++)
		{
			server::SomeData data;
			data.deserialize(list[i]);
			if (data.create)
				create_object(data, client);
			else
				delete_object(data.id, client);
		}
	}

	void SomeDataWorker::send(ENetPeer *peer)
	{
		if (queue.empty())
			return;

		ENetPacket *packet = enet_packet_create(
			nullptr, 1350,
			ENET_PACKET_FLAG_RELIABLE
		);
		std::memset(packet->data, 0, packet->dataLength);
		kj::ArrayPtr<capnp::word> segment(
			(capnp::word *)packet->data,
			packet->dataLength / sizeof(capnp::word)
		);

		capnp::FlatMessageBuilder root(std::move(segment));
		Action::Builder builder = root.initRoot<Action>();
		auto hello = builder.initData().initHello();

		SomeData data = queue.front();
		hello.setNickname(data.nickname);

		enet_packet_resize(packet,
				root.sizeInWords() * sizeof(capnp::word));
		enet_peer_send(peer, 1, packet);

		queue.pop();
	}

	void SomeDataWorker::append_hello(std::string name)
	{
		queue.emplace(connection::Action::Data::Which::HELLO,
				std::move(name));
	}

	void SomeDataWorker::create_object(const server::SomeData &data,
			Node *client_node)
	{
		if (client_node->find_child("*_" + String::num_uint64(data.id),
						false, false) != nullptr)
			return;
		User *obj = nullptr;
		String obj_type_name;
		using Type = server::SomeDataObjectType;
		switch (data.obj_type)
		{
			case Type::player:
				obj = Object::cast_to<User>(player_scene->instantiate());
				obj_type_name = "player";
			break;

			case Type::enemy:
				obj = Object::cast_to<User>(enemy_scene->instantiate());
				obj_type_name = "enemy";
				obj->set_nickname(std::get<1>(data.data));
			break;

			default:
				;
		}

		if (obj == nullptr)
		{
			print(CUR_PRINT_INFO, "The {} scene is corrupted",
					obj_type_name.utf8().ptr());
			return;
		}

		obj->set_position(data.pos);
		if (data.id != obj->get_id())
		{
			IdPull::instance().reserve_id(data.id);
			obj->set_id(data.id);
		}
		obj->set_name(obj_type_name + '_' + String::num_uint64(data.id));
		print("Add obj with name: {}", String(obj->get_name()).utf8().ptr());

		client_node->add_child(obj);
	}

	void SomeDataWorker::delete_object(int id, Node *client)
	{
		Node *obj = client->find_child("*_" + String::num_uint64(id), false, false);
		if (obj == nullptr)
		{
			print(CUR_PRINT_INFO, "Node with ID:{} not found", id);
			return;
		}
		obj->queue_free();
	}
} //namespace pnd::client