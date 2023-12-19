#pragma once

#include "obj.hxx"
#include "client/dyn_info.hxx"
#include "obj_wrapper.hxx"

#include <gdextension_interface.h>
#include <godot_cpp/classes/character_body3d.hpp>

namespace pnd
{
	class User :
		public godot::CharacterBody3D,
		public Obj
	{
		GDCLASS(User, CharacterBody3D)
	public:
		User();
		User(uint8_t id);
		User(User &&other);
		~User();

		virtual void _ready() override;
		virtual void _physics_process(double delta) override;

		void set_nickname(const godot::String &name);
		const godot::String &get_nickname() const;

		client::DynamicInfo *get_dyn_info() const;
	protected:
		static void _bind_methods();
	private:
		void process_gravity(double delta, godot::Vector3 &new_vel);
		void process_jump(godot::Vector3 &new_vel);
		void process_move(double delta, godot::Vector3 &new_vel);

		godot::String nickname;
		ObjWrapper<client::DynamicInfo> dyn_info;
	};
} //namespace pnd