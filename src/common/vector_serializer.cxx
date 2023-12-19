#include "vector_serializer.hxx"

#include <stdfloat>

namespace pnd
{
	void VectorSerializer::serialize(Vector::Builder builder,
			const godot::Vector3 &vec)
	{
		builder.setX(vec.x);
		builder.setY(vec.y);
		builder.setZ(vec.z);
	}

	void VectorSerializer::deserialize(Vector::Reader reader,
			godot::Vector3 &vec)
	{
		vec.x = reader.getX();
		vec.y = reader.getY();
		vec.z = reader.getZ();
	}

	godot::Vector3 VectorSerializer::deserialize(Vector::Reader reader)
	{
		godot::Vector3 vec;
		deserialize(reader, vec);
		return vec;
	}
} //namespace pnd