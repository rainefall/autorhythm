extends MeshInstance

var target_lane = 0;

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	if Input.is_action_pressed("game_left"):
		target_lane = -1
	elif Input.is_action_pressed("game_right"):
		target_lane = 1
	else:
		target_lane = 0
		
	transform.origin.x = lerp(transform.origin.x, target_lane * -2, 0.5)
	transform.origin.z += 88.2 * delta
