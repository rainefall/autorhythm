extends Node

# zero matrix for "removing" a block
var transform_zero = Transform(Vector3(0,0,0),Vector3(0,0,0),Vector3(0,0,0),Vector3(0,0,0))

# index of next block that the player can collide with
var next_block = 0

# score
var score = 0
# score multiplier
var score_multiplier = 1.0

# object that stores the FMOD Sound and FMOD Channel objects required
# to play the music
var sound

var paused_state = false

# Called when the node enters the scene tree for the first time.
func _ready():
	# load song
	sound = Global.FMODSound.new()
	sound.create(Global.current_lvl_path)
	
	$Blocks.multimesh.instance_count = Global.current_lvl["onsets"].size() / 12
	# currently, this does not work as it sets transform data incorrectly
	#$Blocks.multimesh.set_as_bulk_array(Global.current_lvl["onsets"])
	
	# this is HORRIBLY inefficient
	# but godot has forced my hand, as set_as_bulk_array does not work
	for i in range(0,Global.current_lvl["onsets"].size()/12):
		var offset = i*12
		var t = Transform(	Vector3(Global.current_lvl["onsets"][offset],Global.current_lvl["onsets"][offset+1],Global.current_lvl["onsets"][offset+2]),
							Vector3(Global.current_lvl["onsets"][offset+3],Global.current_lvl["onsets"][offset+4],Global.current_lvl["onsets"][offset+5]),
							Vector3(Global.current_lvl["onsets"][offset+6],Global.current_lvl["onsets"][offset+7],Global.current_lvl["onsets"][offset+8]),
							Vector3(Global.current_lvl["onsets"][offset+9],Global.current_lvl["onsets"][offset+10],Global.current_lvl["onsets"][offset+11]))
		$Blocks.multimesh.set_instance_transform(i, t)
	
	# set initial UI values
	get_node("User Interface/Multiplier").text = "x1.0"
	get_node("User Interface/Score").text = "0"
	get_node("User Interface/Metadata").text = Global.current_lvl["metadata"][1] + " - " + Global.current_lvl["metadata"][0]
	
	# play the music
	sound.play()

# godot doesnt let you directly delete things from an multimesh
# however, setting the instance's transform to be all zeroes makes it disappear
func remove_block(index):
	$Blocks.multimesh.set_instance_transform(index,transform_zero)

# Called once every frame
func _process(delta):
	if paused_state:
		# check for pause button, unpause
		if Input.is_action_just_pressed("game_pause"):
			$"User Interface/PauseMenu".hide()
			sound.unpause()
			paused_state = false
	else:
		# check for pause button, pause
		if Input.is_action_just_pressed("game_pause"):
			$"User Interface/PauseMenu".show()
			sound.pause()
			paused_state = true
		
		# move camera
		$Camera.transform.origin.z = $Player.transform.origin.z - 8
		
		if sound.channel_position() >= Global.current_lvl["metadata"][2] and next_block >= Global.current_lvl["onsets"].size() / 12:
			Global.save_score(score, Global.game_settings["defname"])
			get_tree().change_scene("res://scenes/highscore.tscn")
		
		if next_block < Global.current_lvl["onsets"].size() / 12:
			if $Player.transform.origin.z > Global.current_lvl["onsets"][next_block * 12 + 11] + 2:
				# very much not in range
				# increase block check counter
				next_block += 1
				# reset score multiplier to 1
				score_multiplier = 1.0
				
				# update UI
				get_node("User Interface/Multiplier").text = "x"+str(score_multiplier)
			elif $Player.transform.origin.z > Global.current_lvl["onsets"][next_block * 12 + 11] - 2.5:
				if $Player.transform.origin.x > Global.current_lvl["onsets"][next_block * 12 + 9] - 1.5 && $Player.transform.origin.x < Global.current_lvl["onsets"][next_block * 12 + 9] + 1.5:
					# "remove" block so that the player knows they have hit it
					remove_block(next_block)
					# increase score and multiplier
					score += 10 * score_multiplier
					if score_multiplier < 4.0:
						score_multiplier += 0.1
					# increase block check counter
					next_block += 1
					
					# update UI
					get_node("User Interface/Multiplier").text = "x"+str(score_multiplier)
					get_node("User Interface/Score").text = str(int(score))


func _on_Resume_pressed():
	$"User Interface/PauseMenu".hide()
	sound.unpause()
	paused_state = false


func _on_Exit_pressed():
	get_tree().change_scene("res://scenes/Menus.tscn")
