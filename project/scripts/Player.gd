extends MeshInstance

var target_lane = 0;

var keyb = true;


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	# input code
	if Input.is_action_pressed("game_left"):
		keyb = true
		target_lane = -1
	elif Input.is_action_pressed("game_right"):
		keyb = true
		target_lane = 1
	elif keyb:
		target_lane = 0
	
	# 0.5 represents the intended linear interpolation value at 60 frames per
	# second, 0.0167 represents the delta time between frames at 60 frames per
	# second. these are multiplied by the actual delta time to get a movement
	# speed that looks the same at all frame rates
	transform.origin.x = lerp(transform.origin.x, target_lane * -2, (0.5 / 0.0167) * delta)
	# 88.2 is a constant that represents the distance between two onsets in a
	# song that are exactly one second apart
	# this is multiplied with delta time to work out the distance that must
	# be travelled to stay in sync with the song
	transform.origin.z += 88.2 * delta
	# this solution isnt perfect, if there is a large lag spike (very unlikely
	# on any modern hardware) the game will jump forward, unlike in audiosurf
	# where the game temporarily increases in speed to catch up with the 
	# audio playback

# touch screen input, does not work on windows but it might work on android?
# not that i'm porting this to android
func _unhandled_input(event):
	pass
	if event is InputEventScreenTouch:
		if event.pressed:
			keyb = false
			var w = get_viewport().size.x
			if event.position.x > w/2:
				target_lane = 1
			else:
				target_lane = -1
		else:
			target_lane = 0
