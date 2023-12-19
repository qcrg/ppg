#pragma once

#include "obj.hxx"

#include <enet/enet.h>
#include <queue>
#include <godot_cpp/variant/string_name.hpp>
#include <chrono>

namespace pnd::server
{
	struct ChatMessage
	{
		ChatMessage();
		ChatMessage(const Obj &obj, const char *msg);

		int user_id;
		std::chrono::seconds timestamp;
		std::chrono::milliseconds timestamp_mil;
		godot::String text;
	};

	class ChatWorker
	{
	public:
		void operator()(const ENetEvent &event);
		void send(ENetHost *host);
		void append(ChatMessage &&msg);
	private:
		std::queue<ChatMessage> queue;
	};
} //namespace pnd::server