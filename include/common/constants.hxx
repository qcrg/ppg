#pragma once

#include <cstdint>
#include <cstddef>

namespace pnd::constants
{
	// constexpr const double gravity = 200;
	constexpr const double speed = 200;
	constexpr const double sprint_multi = 1.5;
	constexpr const double sprint_speed = speed * sprint_multi;
	constexpr const double jump_force = 7.8;
	constexpr const double gravity = 9.8;
} //namespace pnd::constants