#pragma once

#include <godot_cpp/core/memory.hpp>

namespace pnd
{
	template<typename T>
	struct ObjWrapper
	{
		using PtrT = T *;
		ObjWrapper() { ptr = memnew(T); }
		~ObjWrapper() { if (ptr) godot::memdelete(ptr); }
		operator bool() const { return !!ptr; }
		operator PtrT() const { return ptr; }
		PtrT operator*() const { return ptr; }
		PtrT operator->() const { return ptr; }
		PtrT operator=(PtrT other) { return ptr = other; }

		PtrT ptr;
	};
} //namespace pnd