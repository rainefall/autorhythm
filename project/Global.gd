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

var game_settings: Dictionary

# Called when the node enters the scene tree for the first time.
func _ready():
	var fileops:File = File.new()
	var dirops:Directory = Directory.new()
	# make directories for level cache and leaderboard if they dont exist
	if !dirops.dir_exists("user://level_cache"):
		dirops.make_dir("user://level_cache")
	if !dirops.dir_exists("user://local_leaderboard"):
		dirops.make_dir("user://local_leaderboard")
		
	# check if settings file exists
	if fileops.file_exists("user://game.acf"):
		# read settings from file
		fileops.open("user://game.acf", File.READ)
		var text = fileops.get_as_text()
		game_settings = parse_json(text)
		fileops.close()
	else:
		# create default settings
		game_settings = {
			"foo" : "bar"
		}
		# save them to the settings file
		fileops.open("user://game.acf", File.WRITE)
		fileops.store_line(to_json(game_settings))
		fileops.close()

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
		current_lvl = parse_json(text)
		file.close()
	else:
		# otherwise we generate the level
		current_lvl = LevelGenerator.generate_level(lvl_gen_sound)
		# save it
		file.open("user://level_cache/%s.arl" % fn, File.WRITE)
		file.store_line(to_json(current_lvl))
		file.close()
