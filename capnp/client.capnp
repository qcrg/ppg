@0xb690a90649bf57fb;
# Client

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("client");

using Types = import "./types.capnp";

#-----Chat
struct ChatMessage
{
	content @0: Text;
}

struct ChatPacket
{
	list @0: List(ChatMessage);
}
#----/Chat


#-----Dynamic Information
struct DynInfo
{
	cameraDir @0: Types.Vector;
	buttonFlags: union {
		empty @1: Void;
		data @2: UInt8;
	}
	velocity: union {
		empty @3: Void;
		data @4: Types.Vector;
	}
}

struct DynInfoPacket
{
	list @0: List(DynInfo);
}
#----/Dynamic Information