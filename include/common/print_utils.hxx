#pragma once

#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <format>
#include <boost/current_function.hpp>

namespace pnd
{
	struct PrintInfo
	{
		const char *func{nullptr};
		const char *file{nullptr};
		int line{0};
#define CUR_PRINT_INFO \
		::pnd::PrintInfo{BOOST_CURRENT_FUNCTION, __FILE__, __LINE__}
	};

	template<typename ...Args>
	void print(const PrintInfo &info,
			const std::format_string<Args ...> fmt,
			Args&& ...args)
	{
		godot::internal::gdextension_interface_print_warning_with_message(
			"Descr",
			std::vformat(fmt.get(), std::make_format_args(args...)).c_str(),
			info.func, info.file, info.line, false
		);
	}

	template<typename ...Args>
	void print(const std::format_string<Args ...> fmt,
			Args&& ...args)
	{
		godot::internal::gdextension_interface_print_warning_with_message(
			"Descr",
			std::vformat(fmt.get(), std::make_format_args(args...)).c_str(),
			nullptr, nullptr, 0, false
		);
	}

	template<typename ...Args>
	void print_error(const PrintInfo &info,
			const std::format_string<Args...> fmt,
			Args&& ...args)
	{
		godot::internal::gdextension_interface_print_error_with_message(
			"Descr",
			std::vformat(fmt.get(), std::make_format_args(args...)).c_str(),
			info.func, info.file, info.line, false
		);
	}

	template<typename ...Args>
	void print_error(const std::format_string<Args ...> fmt,
			Args&& ...args)
	{
		godot::internal::gdextension_interface_print_error_with_message(
			"Descr",
			std::vformat(fmt.get(), std::make_format_args(args...)).c_str(),
			nullptr, nullptr, 0, false
		);
	}
} //namespace pnd

template<>
struct std::formatter<godot::String>
{
	constexpr auto parse(std::format_parse_context &ctx)
	{
		return std::formatter<const char *>().parse(ctx);
	}

	auto format(const godot::String &str, std::format_context &ctx)
	{
		return std::formatter<const char *>().format(str.utf8().get_data(), ctx);
	}
};

template<>
struct std::formatter<godot::StringName>
{
	constexpr auto parse(std::format_parse_context &ctx)
	{
		return std::formatter<const char *>().parse(ctx);
	}

	auto format(const godot::StringName &str, std::format_context &ctx)
	{
		return std::formatter<const char *>().format(
				reinterpret_cast<const char *>(str.to_utf8_buffer().ptr()), ctx);
	}
};