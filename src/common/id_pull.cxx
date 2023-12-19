#include "id_pull.hxx"

#include "print_utils.hxx"

#include <unordered_set>
#include <ranges>
#include <algorithm>
#include <boost/current_function.hpp>
#include <string>
#include <godot_cpp/godot.hpp>
#include <format>

namespace debug
{
	template<typename ...Args>
	void print(const std::format_string<Args...> fmt, Args&& ...args)
	{
		printf("%s",
				std::vformat(fmt.get(), std::make_format_args(args...)).c_str());
	}
} //namespace debug

namespace pnd
{
	struct impl::IdPull
	{
		IdPull(const int max_id = UINT8_MAX)
			: max_id{max_id}
		{
			pull.reserve(max_id);
			for (int i = 0; i < max_id; i++)
				pull.insert(i);
		}

		int get_id()
		{
			if (pull.empty())
			{
				print_error(CUR_PRINT_INFO,
						"Failed to generate new ID. All IDs in use. Max IDs: {}",
						max_id);
				return max_id;
			}
			auto begin = pull.begin();
			int id = *begin;
			pull.erase(begin);
			debug::print("ID:{} is generated\n", id);
			return id;
		}

		int reserve_id(int id)
		{
			auto it = pull.find(id);
			debug::print("Trying to reserve ID:{} ", id);
			if (it == pull.end())
			{
				print_error(CUR_PRINT_INFO, "Failed to reserve ID:{}", id);
				debug::print("Fail\n");
				return max_id;
			}
			pull.erase(it);
			debug::print("Success\n");
			return id;
		}

		void refund_id(int id)
		{
			debug::print("Try to refund ID:{} ", id);
			auto it = pull.find(id);
			if (it != pull.end())
			{
				print_error(CUR_PRINT_INFO,
						"Refund ID to pull is failed: {}. Already refunded", id);
				debug::print("Fail\n");
				return;
			}
			pull.insert(id);
			debug::print("Success\n");
		}

		bool id_is_free(int id)
		{
			bool res = pull.find(id) != pull.end();
			debug::print("ID:{} is free: {}\n", id, res);
			return res;
		}

		std::unordered_set<int> pull;
		const int max_id;
	};

	IdPull::IdPull()
	{
		impl = new impl::IdPull();
	}

	IdPull::~IdPull()
	{
		if (impl != nullptr)
			delete impl;
	}

	uint8_t IdPull::get_id()
	{
		return impl->get_id();
	}

	uint8_t IdPull::reserve_id(uint8_t id)
	{
		return impl->reserve_id(id);
	}

	void IdPull::refund_id(uint8_t id)
	{
		impl->refund_id(id);
	}

	bool IdPull::id_is_free(uint8_t id) const
	{
		return impl->id_is_free(id);
	}

	IdPull &IdPull::instance()
	{
		static IdPull pull;
		return pull;
	}

	IdGuard::IdGuard()
		: id{IdPull::instance().get_id()}
	{
	}

	IdGuard::~IdGuard()
	{
		IdPull::instance().refund_id(id);
	}
} //namespace pnd