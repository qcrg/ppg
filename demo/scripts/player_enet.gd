extends User

@export var mouse_speed: float = 0.002

@onready var camera = $Camera3D

var focused: bool = false

func _input(event):
	if event is InputEventMouseButton and \
			event.button_index == MOUSE_BUTTON_LEFT:
		Input.mouse_mode = Input.MOUSE_MODE_CAPTURED
		focused = true
		return
	
	if event is InputEventKey and \
			focused and \
			event.keycode == KEY_ESCAPE:
		Input.mouse_mode = Input.MOUSE_MODE_VISIBLE
		focused = false
		return
		
	if (not event is InputEventMouseMotion) or not focused:
		return
	var mrel = event.relative * mouse_speed
	rotation.y -= mrel.x
	camera.rotation.x = clampf(camera.rotation.x - mrel.y,
		-PI / 2 + 0.01, PI / 2 - 0.01)
	var camera_dir = Vector3(camera.rotation.x, rotation.y, 0)
	get_dyn_info().camera_dir = camera_dir

func set_button_flags():
	var flags = 0
	if (Input.is_action_pressed("jump")):
		flags |= JUMP
	if (Input.is_action_pressed("shoot")):
		flags |= SHOOT
	if (Input.is_action_pressed("move_back")):
		flags |= BACK
	if (Input.is_action_pressed("move_front")):
		flags |= FRONT
	if (Input.is_action_pressed("move_left")):
		flags |= LEFT
	if (Input.is_action_pressed("move_right")):
		flags |= RIGHT
	get_dyn_info().button_flags = flags

func set_vel():
	var transverse_axis = Input.get_axis("move_front", "move_back")
	var longitudinal_axis = Input.get_axis("move_left", "move_right")
	var vel = Vector3(longitudinal_axis, 0, transverse_axis)
	var trans = Transform3D.IDENTITY.rotated(Vector3.UP, -rotation.y)
	vel = vel * trans
	get_dyn_info().velocity = vel
	pass

func _physics_process(delta):
	set_button_flags()
	set_vel()
	pass
