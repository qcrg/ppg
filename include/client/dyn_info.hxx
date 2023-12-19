#pragma once

#include "capnp/client.capnp.h"

#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/classes/object.hpp>

namespace pnd::client
{
	class DynamicInfo : public godot::Object
	{
		GDCLASS(DynamicInfo, Object)
	public:
		DynamicInfo();
		virtual ~DynamicInfo();

		void set_camera_dir(const godot::Vector3 &dir);
		void set_button_flags(uint8_t flags);
		void set_velocity(const godot::Vector3 &vel);

		const godot::Vector3 &get_camera_dir() const;
		const uint8_t &get_button_flags() const;
		const godot::Vector3 &get_velocity() const;

		void serialize(::client::DynInfo::Builder builder) const;
		void deserialize(::client::DynInfo::Reader reader);
	protected:
		static void _bind_methods();
	private:
		godot::Vector3 camera_dir;
		uint8_t button_flags;
		godot::Vector3 velocity;
	};
} //namespace pnd::client