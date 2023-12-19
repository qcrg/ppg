#pragma once

namespace pnd
{

#define REG_SET_GET(klass, name) \
	ClassDB::bind_method(D_METHOD("set_" #name, #name), \
			&klass::set_##name); \
	ClassDB::bind_method(D_METHOD("get_" #name), \
			&klass::get_##name); \

#define REG_PROP(klass, name, type) \
	REG_SET_GET(klass, name) \
	ADD_PROPERTY(PropertyInfo(type, #name), \
			"set_" #name, "get_" #name);

#define REG_PROP_HINT(klass, name, type, hint, hint_str) \
	REG_SET_GET(klass, name) \
	ADD_PROPERTY(PropertyInfo(type, #name, hint, hint_str), \
			"set_" #name, "get_" #name);

#define REG_PROP_USAGE(klass, name, type, usage) \
	REG_SET_GET(klass, name) \
	ADD_PROPERTY(PropertyInfo(type, #name, PROPERTY_HINT_NONE, "", usage), \
			"set_" #name, "get_" #name);

} //namespace pnd