#include "user.hxx"

#include "button_flags.hxx"
#include "constants.hxx"
#include "reg_prop.hxx"
#include "print_utils.hxx"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/transform3d.hpp>
#include <algorithm>
#include <cmath>
#include <limits>

using namespace godot;

VARIANT_ENUM_CAST(pnd::ButtonFlags);

namespace pnd
{
	User::User()
	{
	}

	User::User(uint8_t id)
		: Obj{id}
	{
	}

	User::User(User &&other)
		: Obj{other.release_id()}
	{
		*this = std::move(other);
		dyn_info = other.dyn_info;
		other.dyn_info = nullptr;
	}

	User::~User()
	{
	}

	void
	User::_ready()
	{
		set_physics_process(!godot::Engine::get_singleton()->is_editor_hint());
	}

	void User::_physics_process(double delta)
	{
		Vector3 new_vel = get_velocity();
		process_gravity(delta, new_vel);
		process_jump(new_vel);
		process_move(delta, new_vel);
		set_velocity(new_vel);
		move_and_slide();
	}

	void User::process_gravity(double delta, Vector3 &new_vel)
	{
		new_vel.y = std::clamp<real_t>(new_vel.y - delta * constants::gravity,
				3 * -constants::gravity, std::numeric_limits<real_t>::max());
	}

	void User::process_jump(Vector3 &new_vel)
	{
		if (!(has_button_flag(get_dyn_info()->get_button_flags(),
				ButtonFlags::JUMP) &&
				is_on_floor()))
			return;
		new_vel.y = constants::jump_force;
	}

	void User::process_move(double delta, Vector3 &new_vel)
	{
		using bf = ButtonFlags;
		double speed = has_button_flag(get_dyn_info()->get_button_flags(),
				ButtonFlags::SPRINT) ? constants::sprint_speed : constants::speed;
		// new_vel.x = get_dyn_info()->get_velocity().x * speed * delta;
		// new_vel.z = get_dyn_info()->get_velocity().z * speed * delta;
		float t_axis = get_axis(get_dyn_info()->get_button_flags(),
				bf::FRONT, bf::BACK);
		float l_axis = get_axis(get_dyn_info()->get_button_flags(),
				bf::LEFT, bf::RIGHT);
		Vector3 vel(l_axis * speed * delta,
			0,
			t_axis * speed * delta
		);
		Transform3D trans;
		trans.rotate(Vector3(0, 1, 0), get_dyn_info()->get_camera_dir().y);
		vel = trans.xform(vel);
		new_vel.x = vel.x;
		new_vel.z = vel.z;
	}

	void User::set_nickname(const godot::String &name)
	{
		nickname = name;
	}

	const godot::String &User::get_nickname() const
	{
		return nickname;
	}

	client::DynamicInfo *User::get_dyn_info() const
	{
		return dyn_info;
	}

	void User::_bind_methods()
	{
		BIND_ENUM_CONSTANT(FRONT);
		BIND_ENUM_CONSTANT(BACK);
		BIND_ENUM_CONSTANT(LEFT);
		BIND_ENUM_CONSTANT(RIGHT);
		BIND_ENUM_CONSTANT(SPRINT);
		BIND_ENUM_CONSTANT(JUMP);
		BIND_ENUM_CONSTANT(SHOOT);

		ClassDB::bind_method(D_METHOD("get_dyn_info"),
				&User::get_dyn_info);
	}
} //namespace pnd