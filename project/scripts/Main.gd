extends Node

onready var FMODSound = preload("res://bin/scripts/FMODSound.gdns").new()
onready var LevelGenerator = preload("res://bin/scripts/LevelGenerator.gdns").new()

var lvl: Dictionary;

# Called when the node enters the scene tree for the first time.
func _ready():
	# load song & generate level
	FMODSound.create("E:/Godot/AutoRhythm/project/test.ogg")
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

	# play the music
	#FMODSound.play()
