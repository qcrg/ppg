#pragma once

#include "user.hxx"

#include <enet/enet.h>
#include <queue>
#include <functional>

namespace pnd::server
{
	class DynamicDataWorker
	{
	public:
		void operator()(const ENetEvent &event);
		void send(ENetHost *host);
		void append(User *user);
	private:
		std::queue<User *> queue;
	};
} //namespace pnd::server