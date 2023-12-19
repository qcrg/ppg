#include "server/chat_worker.hxx"

#include "capnp/client.capnp.h"
#include "capnp/server.capnp.h"
#include "channels.hxx"

#include <capnp/message.h>
#include <chrono>
#include <ctime>
#include <cassert>

namespace pnd::server
{
	ChatMessage::ChatMessage()
		: user_id{UINT8_MAX}
	{
	}

	ChatMessage::ChatMessage(const Obj &obj, const char *msg)
		: user_id{obj.get_id()}
		, text{msg}
	{
		namespace sc = std::chrono;
		sc::milliseconds now = sc::duration_cast<sc::milliseconds>(
				sc::system_clock::now().time_since_epoch());
		timestamp = sc::duration_cast<sc::seconds>(now);
		auto mil_double = (now - timestamp) / 3.90625;
		timestamp_mil = sc::milliseconds{static_cast<int64_t>(mil_double.count())};
	}

	void ChatWorker::operator()(const ENetEvent &event)
	{
		Obj *obj = reinterpret_cast<Obj *>(event.peer->data);
		kj::ArrayPtr<const capnp::word> data(
				reinterpret_cast<const capnp::word *>(event.packet->data),
				event.packet->dataLength / sizeof(capnp::word));
		capnp::SegmentArrayMessageReader root({&data, 1});
		client::ChatPacket::Reader reader = root.getRoot<client::ChatPacket>();

		auto list = reader.getList();
		for (const ::client::ChatMessage::Reader &unit : list)
			if (unit.hasContent())
				append(ChatMessage(*obj, unit.getContent().cStr()));
	}

	void serialize_message(::server::ChatMessage::Builder builder,
			const ChatMessage &msg)
	{
		builder.setUserId(msg.user_id);
		builder.setTimestamp(msg.timestamp.count());
		builder.setTimestampMil(msg.timestamp_mil.count());
		capnp::Text::Builder text = builder.initContent(msg.text.length());
		std::memcpy(text.begin(),
				msg.text.utf8().ptr(),
				text.size());
	}

	void ChatWorker::send(ENetHost *host)
	{
		if (queue.empty())
			return;

		ENetPacket *packet = enet_packet_create(nullptr,
				ENET_HOST_DEFAULT_MTU - ENET_HOST_DEFAULT_MTU % sizeof(capnp::word),
				ENET_PACKET_FLAG_RELIABLE);
		assert(packet);
		std::memset(packet->data, 0, packet->dataLength);
		
		kj::ArrayPtr<capnp::word> segment(
			reinterpret_cast<capnp::word *>(packet->data),
			packet->dataLength / sizeof(capnp::word)
		);
		capnp::FlatMessageBuilder root(segment);

		auto builder = root.initRoot<::server::ChatPacket>();
		auto list = builder.initList(queue.size());
		for (uint i = 0; i < list.size(); i++, queue.pop())
			serialize_message(list[i], queue.front());

		assert(0 == enet_packet_resize(packet,
				root.sizeInWords() * sizeof(capnp::word)));
		enet_host_broadcast(host, static_cast<int>(Channels::chat), packet);
	}

	void ChatWorker::append(ChatMessage &&msg)
	{
		queue.push(std::move(msg));
	}
} // namespace pnd::server