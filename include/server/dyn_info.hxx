#pragma once

#include "capnp/server.capnp.h"

#include <godot_cpp/variant/vector3.hpp>

namespace pnd::server
{
	class DynamicInfo
	{
	public:
		void set_id(uint8_t id);
		void set_pos(const godot::Vector3 &pos);
		void set_camera_dir(const godot::Vector3 &dir);
		void set_velocity(const godot::Vector3 &vel);

		const uint8_t &get_id() const;
		const godot::Vector3 &get_pos() const;
		const godot::Vector3 &get_camera_dir() const;
		const godot::Vector3 &get_velocity() const;

		void serialize(::server::DynInfo::Builder builder) const;
		void deserialize(::server::DynInfo::Reader reader);
	private:
		uint8_t id;
		godot::Vector3 pos;
		godot::Vector3 camera_dir;
		godot::Vector3 velocity;
	};
} //namespace pnd::server