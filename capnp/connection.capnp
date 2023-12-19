@0xaae08bfb967b018d;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("connection");

struct HelloData
{
	nickname @0: Text;
}

struct ReadyData
{
	empty @0: Void;
}

struct OkData
{
	empty @0: Void;
}

struct Action
{
	data: union {
		hello @0: HelloData;
		ready @1: ReadyData;
		ok @2: OkData;
	}
}