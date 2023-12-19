#include "client/dyn_info.hxx"

#include "vector_serializer.hxx"
#include "reg_prop.hxx"

#include <cmath>
#include <algorithm>
#include <godot_cpp/core/class_db.hpp>


//FIXME
#include "print_utils.hxx"

namespace pnd::client
{
	DynamicInfo::DynamicInfo()
		: camera_dir{0, 0, 0}
		, button_flags{0}
		, velocity{0, 0, 0}
	{
	}

	DynamicInfo::~DynamicInfo()
	{
	}

	void DynamicInfo::serialize(::client::DynInfo::Builder builder) const
	{
		VectorSerializer::serialize(builder.initCameraDir(), camera_dir);
		if (button_flags)
		{
			auto bflags = builder.initButtonFlags();
			bflags.setData(button_flags);
		}
		if (velocity != godot::Vector3(0, 0, 0))
		{
			auto vel = builder.initVelocity();
			VectorSerializer::serialize(vel.initData(), velocity);
		}
	}

	void DynamicInfo::deserialize(::client::DynInfo::Reader reader)
	{
		VectorSerializer vs;
		vs.deserialize(reader.getCameraDir(), camera_dir);
		if (reader.getButtonFlags().isEmpty())
			set_button_flags(0);
		else
			set_button_flags(reader.getButtonFlags().getData());
		if (reader.getVelocity().isEmpty())
			set_velocity(godot::Vector3(0, 0, 0));
		else
			set_velocity(vs.deserialize(reader.getVelocity().getData()));
	}

	void DynamicInfo::set_camera_dir(const godot::Vector3 &dir)
	{
		camera_dir.x = std::clamp(static_cast<double>(dir.x),
			M_PI / 2 + 0.01, M_PI / 2 - 0.01);
		camera_dir.y = dir.y;
	}

	void DynamicInfo::set_button_flags(uint8_t flags)
	{
		button_flags = flags;
	}

	void DynamicInfo::set_velocity(const godot::Vector3 &vel)
	{
		velocity = vel.normalized();
	}

	const godot::Vector3 &DynamicInfo::get_camera_dir() const
	{
		return camera_dir;
	}

	const uint8_t &DynamicInfo::get_button_flags() const
	{
		return button_flags;
	}

	const godot::Vector3 &DynamicInfo::get_velocity() const
	{
		return velocity;
	}

	void DynamicInfo::_bind_methods()
	{
		using namespace godot;
		REG_PROP(DynamicInfo, camera_dir, Variant::VECTOR3);
		REG_PROP_USAGE(DynamicInfo, button_flags, Variant::INT,
				PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_CLASS_IS_BITFIELD);
		REG_PROP(DynamicInfo, velocity, Variant::VECTOR3);
	}
} //namespace pnd::client