#include "obj.hxx"

#include "id_pull.hxx"

namespace pnd
{
	Obj::Obj()
		: id{IdPull::instance().get_id()}
	{
	}

	Obj::Obj(uint8_t id)
		: id{IdPull::instance().reserve_id(id)}
	{
	}

	Obj::Obj(Obj &&other)
		: id{other.id}
	{
		other.id = UINT8_MAX;
	}

	Obj::~Obj()
	{
		if (id != UINT8_MAX)
			IdPull::instance().refund_id(id);
	}

	void Obj::set_id(uint8_t new_id)
	{
		IdPull &pull = IdPull::instance();
		pull.refund_id(id);
		id = new_id;
	}

	uint8_t Obj::get_id() const
	{
		return id;
	}

	uint8_t Obj::release_id()
	{
		uint8_t res = id;
		id = UINT8_MAX;
		return res;
	}
} //namespace pnd

uint64_t std::hash<pnd::Obj>::operator()(const pnd::Obj &obj) const
{
	return std::hash<decltype(obj.get_id())>()(obj.get_id());
}
