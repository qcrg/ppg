#pragma once

#include <cstdint>

namespace pnd
{
	namespace impl
	{
		struct IdPull;
	} //namespace impl

	struct IdPull
	{
		/**
		 * @brief Get the id object
		 * 
		 * @return uint8_t -1(255(UINT8_MAX)) if IDs is full
		 */
		uint8_t get_id();
		uint8_t reserve_id(uint8_t id);
		void refund_id(uint8_t id);
		bool id_is_free(uint8_t id) const;
		static IdPull &instance();
		~IdPull();
	private:
		IdPull();
		impl::IdPull *impl;
	};

	struct IdGuard
	{
		const int id;
		IdGuard();
		~IdGuard();
	};
} //namespace pnd