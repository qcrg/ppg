#include "server/some_data.hxx"

#include "vector_serializer.hxx"

namespace pnd::server
{
	void SomeData::serialize(::server::Obj::Builder builder) const
	{
		builder.setId(id);
		auto type = builder.initType();
		if (!create)
		{
			type.setDestroy();
			return;
		}

		auto obj = type.initCreate();
		VectorSerializer::serialize(obj.initPos(), pos);
		auto obju = obj.initObject();
		switch (obj_type)
		{
			case SomeDataObjectType::none:
			break;

			case SomeDataObjectType::player:
			{
				auto player = obju.initPlayer();
				(void)player;
			}
			break;

			case SomeDataObjectType::enemy:
			{
				auto enemy = obju.initEnemy();
				godot::String nname = std::get<1>(data);
				auto name_br = enemy.initNickname(nname.length());
				std::memcpy(name_br.begin(), nname.utf8().ptr(), nname.length());
			}
			break;

			case SomeDataObjectType::map:
			{
				auto map = obju.initMap();
				map.setMapId(0);
			}
			break;
		}
	}

	void SomeData::deserialize(::server::Obj::Reader reader)
	{
		using Which = ::server::ObjData::Object::Which;
		id = reader.getId();
		auto type = reader.getType();
		create = type.isCreate();
		if (type.isDestroy())
		{
			obj_type = SomeDataObjectType::none;
			return;
		}
		auto obj = type.getCreate();
		pos = VectorSerializer::deserialize(obj.getPos());
		auto obju = obj.getObject();
		switch (obju.which())
		{
			case Which::PLAYER:
				obj_type = SomeDataObjectType::player;
			break;

			case Which::ENEMY:
				obj_type = SomeDataObjectType::enemy;
				data = godot::String(obju.getEnemy().getNickname().cStr());
			break;

			case Which::MAP:
				obj_type = SomeDataObjectType::map;
			break;
		}
	}
} //namespace pnd::server