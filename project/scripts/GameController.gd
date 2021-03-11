extends Node

# native extensions
# fmod sound, loads song for the level generator then plays it
onready var FMODSound = preload("res://bin/scripts/FMODSound.gdns").new()
# level generator, generates level and stores it as a dictionary
onready var LevelGenerator = preload("res://bin/scripts/LevelGenerator.gdns").new()

# zero matrix for "removing" a block
var transform_zero = Transform(Vector3(0,0,0),Vector3(0,0,0),Vector3(0,0,0),Vector3(0,0,0))
# level data, generated from native code
var lvl: Dictionary

# index of next block that the player can collide with
var next_block = 0

# score
var score = 0
# score multiplier
var score_multiplier = 1.0

# Called when the node enters the scene tree for the first time.
func _ready():
	# load song & generate level
	#FMODSound.create("E:/Godot/AutoRhythm/project/test.mp3")
	FMODSound.create("C:/Users/littl/Desktop/autorhythm/project/test.mp3")
	lvl = LevelGenerator.generate_level(FMODSound)
	
	$Blocks.multimesh.instance_count = lvl["onsets"].size() / 12
	# currently, this does not work as it sets transform data incorrectly
	#$Blocks.multimesh.set_as_bulk_array(lvl["onsets"])
	
	# this is HORRIBLY inefficient
	# but godot has forced my hand, as set_as_bulk_array does not work
	for i in range(0,lvl["onsets"].size()/12):
		var offset = i*12
		var t = Transform(	Vector3(lvl["onsets"][offset],lvl["onsets"][offset+1],lvl["onsets"][offset+2]),
							Vector3(lvl["onsets"][offset+3],lvl["onsets"][offset+4],lvl["onsets"][offset+5]),
							Vector3(lvl["onsets"][offset+6],lvl["onsets"][offset+7],lvl["onsets"][offset+8]),
							Vector3(lvl["onsets"][offset+9],lvl["onsets"][offset+10],lvl["onsets"][offset+11]))
		$Blocks.multimesh.set_instance_transform(i, t)
	
	# set initial UI values
	get_node("User Interface/Multiplier").text = "x1.0"
	get_node("User Interface/Score").text = "0"
	get_node("User Interface/Metadata").text = lvl["metadata"][1] + " - " + lvl["metadata"][0]
	
	# play the music
	FMODSound.play()

# godot doesnt let you directly delete things from an multimesh
# however, setting the instance's transform to be all zeroes makes it disappear
func remove_block(index):
	$Blocks.multimesh.set_instance_transform(index,transform_zero)

# Called once every frame
func _process(_delta):
	if next_block < lvl["onsets"].size() / 12:
		if $Player.transform.origin.z > lvl["onsets"][next_block * 12 + 11] + 2:
			# very much not in range
			# increase block check counter
			next_block += 1
			# reset score multiplier to 1
			score_multiplier = 1.0
			
			# update UI
			get_node("User Interface/Multiplier").text = "x"+str(score_multiplier)
		elif $Player.transform.origin.z > lvl["onsets"][next_block * 12 + 11] - 2:
			if $Player.transform.origin.x > lvl["onsets"][next_block * 12 + 9] - 1 && $Player.transform.origin.x < lvl["onsets"][next_block * 12 + 9] + 1:
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
