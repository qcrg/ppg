#include "server/dynamic_data_worker.hxx"

#include "capnp/client.capnp.h"
#include "capnp/server.capnp.h"
#include "client/dyn_info.hxx"
#include "server/dyn_info.hxx"
#include "channels.hxx"
#include "print_utils.hxx"

#include <capnp/message.h>
#include <cassert>

namespace pnd::server
{
	void DynamicDataWorker::operator()(const ENetEvent &event)
	{
		User *user = reinterpret_cast<User *>(event.peer->data);
		kj::ArrayPtr<const capnp::word> segment(
			reinterpret_cast<const capnp::word *>(event.packet->data),
			event.packet->dataLength / sizeof(capnp::word)
		);

		capnp::SegmentArrayMessageReader root({&segment, 1});
		::client::DynInfoPacket::Reader reader =
			root.getRoot<::client::DynInfoPacket>();

		auto list = reader.getList();
		//FIXME
		// print("Server receive\n"
		// 		"\t   data length: {}\n"
		// 		"\t   segment size: {}\n"
		// 		"\t   received from [{}]; list size: {}\n"
		// 		"\t   reader has list: {}\n",
		// 		event.packet->dataLength,
		// 		root.sizeInWords() * sizeof(capnp::word),
		// 		user->get_id(),
		// 		list.size(),
		// 		reader.hasList());
		if (list.size() == 0)
			return;
		user->get_dyn_info()->deserialize(list[0]);
	}

	void DynamicDataWorker::send(ENetHost *host)
	{
		if (queue.empty())
			return;

		ENetPacket *packet = enet_packet_create(nullptr,
				ENET_HOST_DEFAULT_MTU,
				0);
		assert(packet);
		std::memset(packet->data, 0, packet->dataLength);

		kj::ArrayPtr<capnp::word> segment(
			reinterpret_cast<capnp::word *>(packet->data),
			packet->dataLength / sizeof(capnp::word)
		);
		capnp::FlatMessageBuilder root(segment);
		::server::DynInfoPacket::Builder builder =
			root.initRoot<::server::DynInfoPacket>();

		auto list = builder.initList(queue.size());
		// print("Server send:\n"
		// 	"\t   queue size: {}\n"
		// 	"\t   list size: {}\n"
		// 	,
		// 	queue.size(),
		// 	list.size()
		// );
		for (uint i = 0; i < list.size(); i++, queue.pop())
		{
			const User &user = *queue.front();
			server::DynamicInfo info;
			info.set_id(user.get_id());
			info.set_pos(user.get_position());
			info.set_camera_dir(user.get_dyn_info()->get_camera_dir());
			info.set_velocity(user.get_velocity());
			info.serialize(list[i]);
		}
		assert(0 == enet_packet_resize(packet,
				root.sizeInWords() * sizeof(capnp::word)));
		enet_host_broadcast(host, static_cast<int>(Channels::dynamic_data), packet);
	}

	void DynamicDataWorker::append(User *user)
	{
		queue.push(user);
	}
} //namespace pnd::server