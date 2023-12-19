#include "client/dyn_data_worker.hxx"

#include "server/dyn_info.hxx"
#include "capnp/server.capnp.h"
#include "print_utils.hxx"
#include "user.hxx"
#include "channels.hxx"

#include <enet/enet.h>
#include <capnp/message.h>

namespace pnd::client
{
	using namespace godot;
	using namespace capnp;
	void
	DynDataWorker::operator()(const ENetEvent &event, godot::Node *client_node)
	{
		kj::ArrayPtr<const word> segment(
			reinterpret_cast<const word *>(event.packet->data),
			event.packet->dataLength / sizeof(word)
		);

		SegmentArrayMessageReader root({&segment, 1});
		::server::DynInfoPacket::Reader reader =
				root.getRoot<::server::DynInfoPacket>();
		auto list = reader.getList();
		// print("Client receive:\n"
		// 		"\t   Has list: {}\n"
		// 		"\t   List size: {}\n"
		// 		"\t   packet length: {}\n",
		// 		reader.hasList(),
		// 		list.size(),
		// 		event.packet->dataLength);
		for (uint i = 0; i < list.size(); i++)
		{
			server::DynamicInfo info;
			info.deserialize(list[i]);
			User *obj = Object::cast_to<User>(client_node->find_child("*_" +
							String::num_uint64(info.get_id()), false, false));
			if (obj == nullptr)
			{
				print(CUR_PRINT_INFO, "Object with ID:{} not found",
						info.get_id());
				continue;
			}
			DynamicInfo *obj_info = obj->get_dyn_info();
			obj->set_position(info.get_pos());
			obj_info->set_velocity(info.get_velocity());
			if (obj->get_name().match("enemy*"))
			{
				auto cdir = info.get_camera_dir();
				obj_info->set_camera_dir(cdir);
				obj->set_rotation({0, cdir.y, 0});
			}
		}
	}

	void
	DynDataWorker::send(ENetPeer *peer)
	{
		if (queue.empty())
			return;

		ENetPacket *packet = enet_packet_create(nullptr,
				ENET_HOST_DEFAULT_MTU, 0);
		std::memset(packet->data, 0, packet->dataLength);
		kj::ArrayPtr<word> segment(
			reinterpret_cast<word *>(packet->data),
			packet->dataLength / sizeof(word)
		);

		FlatMessageBuilder root(segment);
		::client::DynInfoPacket::Builder builder =
				root.initRoot<::client::DynInfoPacket>();
		auto list = builder.initList(queue.size());
		// print("Client send:\n"
		// 	"\t   queue size: {}\n"
		// 	"\t   list size: {}\n",
		// 	queue.size(),
		// 	list.size()
		// );
		for (uint i = 0; i < list.size(); i++, queue.pop())
		{
			DynamicInfo &info = queue.front();
			info.serialize(list[i]);
		}

		enet_packet_resize(packet, root.sizeInWords() * sizeof(word));
		enet_peer_send(peer, static_cast<int>(Channels::dynamic_data), packet);
	}

	void
	DynDataWorker::append_info(const DynamicInfo &info)
	{
		queue.push(info);
		// print("Info appended: \n\t"
		// 		"Button Flags: {}", info.get_button_flags());
	}
} //namespace pnd::client