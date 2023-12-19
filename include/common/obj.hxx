#pragma once

#include <cstdint>
#include <functional>
#include <boost/noncopyable.hpp>

namespace pnd
{
	class Obj : public boost::noncopyable
	{
	public:
		Obj();
		Obj(uint8_t id);
		Obj(Obj &&obj);
		~Obj();

		void set_id(uint8_t id);
		uint8_t get_id() const;
		uint8_t release_id();
	private:
		int id;
	};
} //namespace pnd

template<>
struct std::hash<pnd::Obj>
{
	uint64_t operator()(const pnd::Obj &obj) const;
};