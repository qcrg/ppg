extends Node3D

var client = preload("res://scenes/debug_client.tscn")
var server = preload("res://scenes/debug_server.tscn")

func _ready():
	for arg in OS.get_cmdline_args():
		if arg == "--server":
			add_child(server.instantiate())
			return
	add_child(client.instantiate())
