extends CharacterBody3D

@onready var camera = $Camera3D

@export var mouse_speed: float = 0.002
@export var speed: float = 200
@export var sprint_multi: float = 1.5
@export var jump_force: float = 8
@export var gravity: float = 10
@export var use_movement: bool = true

@export var resource: PackedScene

func _ready():
	Input.mouse_mode = Input.MOUSE_MODE_CAPTURED
	pass

func _process(_delta):
	pass

func _input(event):
	if (not event is InputEventMouseMotion) or \
		not use_movement:
		return
	var mrel = event.relative * mouse_speed
	rotation.y -= mrel.x
	camera.rotation.x = clampf(camera.rotation.x - mrel.y,
		-PI / 2 + 0.01, PI / 2 - 0.01)

func process_gravity(delta):
	velocity.y -= delta * gravity

func process_jump():
	if not Input.is_action_pressed("jump") or not is_on_floor():
		return
	velocity.y = jump_force

func process_move(delta):
	var cur_speed
	if Input.is_action_pressed("sprint"):
		cur_speed = speed * sprint_multi
	else:
		cur_speed = speed
	var transverse_axis = Input.get_axis("move_front", "move_back")
	var longitudinal_axis = Input.get_axis("move_left", "move_right")
	var vel = Vector3(longitudinal_axis * cur_speed * delta, 0,
		transverse_axis * cur_speed * delta)
	var trans = Transform3D.IDENTITY.rotated(Vector3.UP, -rotation.y)
	vel = vel * trans
	velocity.x = vel.x
	velocity.z = vel.z

func _physics_process(delta):
	process_gravity(delta)
	if use_movement:
		process_jump()
		process_move(delta)
	move_and_slide()
