#pragma once

#include "capnp/server.capnp.h"

#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <variant>

namespace pnd::server
{
	enum class SomeDataObjectType
	{
		none,
		player,
		enemy,
		map
	};

	using None = struct None {};
	using Nickname = godot::String;

	struct SomeData
	{
		bool create;
		uint8_t id;
		godot::Vector3 pos;
		SomeDataObjectType obj_type;
		std::variant<None, Nickname> data;

		void serialize(::server::Obj::Builder builder) const;
		void deserialize(::server::Obj::Reader reader);
	};
} //namespace pnd::server