@0xbdefe82797bc2bd1;
# Server

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("server");

using Types = import "./types.capnp";

#-----Chat
struct ChatMessage
{
	userId @0: UInt8;
	timestamp @1: UInt32;
	timestampMil @2: UInt8;
	content @3: Text;
}

struct ChatPacket
{
	list @0: List(ChatMessage);
}
#----/Chat


#-----Dynamic Information
struct DynInfo
{
	id @0: UInt8;
	pos @1: Types.Vector;
	cameraDir @2: Types.Vector;
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


#-----Object Creation/Destruction
struct PlayerObjData
{
	empty @0: Void;
}

struct EnemyObjData
{
	nickname @0: Text;
}

struct MapObjData
{
	mapId @0: UInt8;
}

struct ObjData
{
	pos @0: Types.Vector;
	object: union {
		player @1: PlayerObjData;
		enemy @2: EnemyObjData;
		map @3: MapObjData;
	}
}

struct Obj
{
	id @0: UInt8;
	type: union {
		destroy @1: Void;
		create @2: ObjData;
	}
}

struct ObjPacket
{
	list @0: List(Obj);
}
#----/Object Creation/Destruction