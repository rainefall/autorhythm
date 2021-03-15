extends Node

# native extensions
# fmod sound, loads song for the level generator then plays it
onready var FMODSound = preload("res://bin/scripts/FMODSound.gdns")
# level generator, generates level and stores it as a dictionary
onready var LevelGenerator = preload("res://bin/scripts/LevelGenerator.gdns").new()

# path to target level
var current_lvl_path: String
# level data
var current_lvl: Dictionary

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func load_level(path):
	var file = File.new()
	current_lvl_path = path
	
	var lvl_gen_sound = FMODSound.new()
	lvl_gen_sound.create(path)
	var fn = lvl_gen_sound.hash()
	
	# check if a level for this song with matching difficulty settings exists
	if file.file_exists("user://level_cache/%s.arl" % fn):
		file.open("user://level_cache/test.arl", File.READ)
		var data = {}
		var text = file.get_as_text()
		data = parse_json(text)
		current_lvl = data
		file.close()
	else:
		# otherwise we generate the level
		current_lvl = LevelGenerator.generate_level(lvl_gen_sound)
		# save it
		file.open("user://level_cache/%s.arl" % fn, File.WRITE)
		file.store_line(to_json(current_lvl))
		file.close()
