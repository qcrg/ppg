#pragma once

#include <cstdint>
#include <concepts>

namespace pnd
{
	enum ButtonFlags
	{
		FRONT = 0x0001,
		BACK = 0x0002,
		LEFT = 0x0004,
		RIGHT = 0x0008,
		SPRINT = 0x0010,
		JUMP = 0x0020,
		SHOOT = 0x0040
	};

	template<std::unsigned_integral T>
	bool has_button_flag(T flags, ButtonFlags flag)
	{
		return static_cast<bool>(flags & static_cast<T>(flag));
	}

	template<std::unsigned_integral T>
	double get_axis(T flags, ButtonFlags negative, ButtonFlags positive)
	{
		double res =
			has_button_flag(flags, positive) - has_button_flag(flags, negative);
		return res;
	}
} //namespace