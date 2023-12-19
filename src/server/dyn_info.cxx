#include "server/dyn_info.hxx"

#include "vector_serializer.hxx"

namespace pnd::server
{
	void DynamicInfo::set_id(uint8_t id)
	{
		this->id = id;
	}

	void DynamicInfo::set_pos(const godot::Vector3 &pos)
	{
		this->pos = pos;
	}

	void DynamicInfo::set_camera_dir(const godot::Vector3 &dir)
	{
		camera_dir = dir;
	}

	void DynamicInfo::set_velocity(const godot::Vector3 &vel)
	{
		velocity = vel;
	}

	const uint8_t &DynamicInfo::get_id() const
	{
		return id;
	}

	const godot::Vector3 &DynamicInfo::get_pos() const
	{
		return pos;
	}

	const godot::Vector3 &DynamicInfo::get_camera_dir() const
	{
		return camera_dir;
	}

	const godot::Vector3 &DynamicInfo::get_velocity() const
	{
		return velocity;
	}

	void DynamicInfo::serialize(::server::DynInfo::Builder builder) const
	{
		VectorSerializer vs;
		builder.setId(id);
		vs.serialize(builder.initPos(), pos);
		vs.serialize(builder.initCameraDir(), camera_dir);
		auto vel = builder.initVelocity();
		if (velocity == godot::Vector3(0, 0, 0))
			vel.setEmpty();
		else
			vs.serialize(vel.initData(), velocity);
	}

	void DynamicInfo::deserialize(::server::DynInfo::Reader reader)
	{
		VectorSerializer vs;
		set_id(reader.getId());
		set_pos(vs.deserialize(reader.getPos()));
		set_camera_dir(vs.deserialize(reader.getCameraDir()));
		auto vel = reader.getVelocity();
		godot::Vector3 vel_val;
		if (vel.isData())
			vs.deserialize(vel.getData(), vel_val);
		else
			vel_val.zero();
		set_velocity(vel_val);
	}
} //namespace pnd::server