#include "vector_serializer.hxx"

#include <stdfloat>

namespace pnd
{
	void VectorSerializer::serialize(Vector::Builder builder,
			const godot::Vector3 &vec)
	{
		auto con = [](real_t c) {
			std::float16_t f = static_cast<std::float16_t>(c);
			return *reinterpret_cast<uint16_t *>(&f);
		};
		builder.setX(con(vec.x));
		builder.setY(con(vec.y));
		builder.setZ(con(vec.z));
	}

	void VectorSerializer::deserialize(Vector::Reader reader,
			godot::Vector3 &vec)
	{
		auto con = [](uint16_t c) {
			return static_cast<real_t>(*reinterpret_cast<std::float16_t *>(&c));
		};
		vec.x = con(reader.getX());
		vec.y = con(reader.getY());
		vec.z = con(reader.getZ());
	}

	godot::Vector3 VectorSerializer::deserialize(Vector::Reader reader)
	{
		godot::Vector3 vec;
		deserialize(reader, vec);
		return vec;
	}
} //namespace pnd