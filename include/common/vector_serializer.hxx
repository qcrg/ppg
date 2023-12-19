#pragma once

#include "capnp/types.capnp.h"

#include <godot_cpp/variant/vector3.hpp>

namespace pnd
{
	struct VectorSerializer
	{
		static void serialize(Vector::Builder builder, const godot::Vector3 &vec);
		static void deserialize(Vector::Reader reader, godot::Vector3 &vec);
		static godot::Vector3 deserialize(Vector::Reader reader);
	};
} //namespace pnd