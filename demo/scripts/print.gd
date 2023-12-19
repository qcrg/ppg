extends Node2D

func print_0():
	print("physics")


func print_1():
	print("process")


# Called when the node enters the scene tree for the first time.
func _ready():
	var tree = get_tree()
	var flag = Object.CONNECT_ONE_SHOT
	#var flag = 0
	tree.physics_frame.connect(print_0, flag)
	tree.process_frame.connect(print_1, flag)
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta):
	pass


func _physics_process(_delta):
	pass
