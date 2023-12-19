#include "server/some_data_worker.hxx"

#include "capnp/connection.capnp.h"
#include "capnp/server.capnp.h"
#include "channels.hxx"

#include <capnp/message.h>
#include <cassert>

namespace pnd::server
{
	using namespace connection;
	using Which = Action::Data::Which;

	ENetPacket *create_packet(std::queue<SomeData> &msgs);

	bool SomeDataWorker::operator()(const ENetEvent &event)
	{
		User *user = reinterpret_cast<User *>(event.peer->data);

		kj::ArrayPtr<const capnp::word> segment(
				reinterpret_cast<capnp::word *>(event.packet->data),
				event.packet->dataLength / sizeof(capnp::word));
		capnp::SegmentArrayMessageReader root({&segment, 1});

		Action::Reader reader = root.getRoot<Action>();
		Action::Data::Reader data_rd = reader.getData();
		if (data_rd.which() != Which::HELLO)
			return false;

		HelloData::Reader hello_rd = data_rd.getHello();
		capnp::Text::Reader text = hello_rd.getNickname();
		std::string nickname(text.cStr());
		user->set_nickname(text.cStr());

		append_enemy_for_all(*user);
		return true;
	}

	void SomeDataWorker::send(ENetHost *host)
	{
		ENetPacket *packet;
		for (auto &[peer, some_data] : queue)
		{
			packet = create_packet(some_data);
			if (packet == nullptr)
				continue;
			enet_peer_send(peer, static_cast<int>(Channels::some_data), packet);
		}
		queue.clear();

		packet = create_packet(broadcast_queue);
		if (packet == nullptr)
			return;
		enet_host_broadcast(host, static_cast<int>(Channels::some_data), packet);
	}

	ENetPacket *create_packet(std::queue<SomeData> &msgs)
	{
		if (msgs.empty())
			return nullptr;

		ENetPacket *packet = enet_packet_create(nullptr,
				ENET_HOST_DEFAULT_MTU - ENET_HOST_DEFAULT_MTU % sizeof(capnp::word),
				ENET_PACKET_FLAG_RELIABLE);
		assert(packet);
		std::memset(packet->data, 0, packet->dataLength);

		kj::ArrayPtr<capnp::word> segment(
				reinterpret_cast<capnp::word *>(packet->data),
				packet->dataLength / sizeof(capnp::word)
		);
		capnp::FlatMessageBuilder root(std::move(segment));

		::server::ObjPacket::Builder builder = root.initRoot<::server::ObjPacket>();
		auto list = builder.initList(msgs.size());

		for (uint i = 0; i < list.size(); i++, msgs.pop())
		{
			const SomeData &data = msgs.front();
			data.serialize(list[i]);
		}
		
		assert(0 == enet_packet_resize(packet,
				root.sizeInWords() * sizeof(capnp::word)));
		return packet;
	}

	SomeData get_some_data(const User &user)
	{
		SomeData data;
		data.id = static_cast<uint8_t>(user.get_id());
		data.pos = user.get_position();
		return data;
	}

	SomeData get_player_some_data(const User &player)
	{
		SomeData data = get_some_data(player);
		data.create = true;
		data.obj_type = SomeDataObjectType::player;
		return data;
	}

	SomeData get_enemy_some_data(const User &enemy)
	{
		SomeData data = get_some_data(enemy);
		data.create = true;
		data.obj_type = SomeDataObjectType::enemy;
		data.data = enemy.get_nickname();
		return data;
	}

	void SomeDataWorker::append_player(ENetPeer *peer)
	{
		User *user = reinterpret_cast<User *>(peer->data);
		queue[peer].push(get_player_some_data(*user));
	}

	void SomeDataWorker::append_enemy(ENetPeer *peer, const User &enemy)
	{
		queue[peer].push(get_enemy_some_data(enemy));
	}

	void SomeDataWorker::append_enemy_for_all(ENetPeer *enemy)
	{
		append_enemy_for_all(*reinterpret_cast<User *>(enemy->data));
	}

	void SomeDataWorker::append_enemy_for_all(const User &enemy)
	{
		broadcast_queue.push(get_enemy_some_data(enemy));
	}

	void SomeDataWorker::destroy_obj_for_all(const Obj &obj)
	{
		SomeData data;
		data.create = false;
		data.id = obj.get_id();
		broadcast_queue.push(std::move(data));
	}
} // namespace pnd::server

constexpr const auto foo =
		ENET_HOST_DEFAULT_MTU - ENET_HOST_DEFAULT_MTU % sizeof(capnp::word);