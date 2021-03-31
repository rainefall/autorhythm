extends Node

# native extensions
# fmod sound, loads song for the level generator then plays it
onready var FMODSound = preload("res://bin/scripts/FMODSound.gdns")
# level generator, generates level and stores it as a dictionary
onready var LevelGenerator = preload("res://bin/scripts/LevelGenerator.gdns").new()
# cubic array, input values and positions for those values 
# and get values from any position by interpolating between them
onready var CubicArray = preload("res://bin/scripts/CubicArray.gdns")

# path to target level
var current_lvl_path: String
# level data
var current_lvl: Dictionary
# current song id
var song_id

# dictionary of settings that are saved to disk
var game_settings: Dictionary
# misc temporary settings
# is the game currently in 2 player mode
var two_player_mode = false;

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
			"generator" : {
				"min_interval" : 250,
				"sensitivity" : 0.3,
				"balance" : 0.5 
			},
			"defname" : "Player",
			"defname2" : "Player 2"
		}
		# save them to the settings file
		save_settings()


func save_settings():
	var fileops:File = File.new()
	fileops.open("user://game.acf", File.WRITE)
	fileops.store_line(to_json(game_settings))
	fileops.close()
	


func load_level(path):
	var file = File.new()
	current_lvl_path = path
	
	var lvl_gen_sound = FMODSound.new()
	lvl_gen_sound.create(path)
	song_id = lvl_gen_sound.hash()
	
	# check if a level for this song with matching difficulty settings exists
	if file.file_exists("user://level_cache/%s.arl" % song_id):
		file.open("user://level_cache/%s.arl" % song_id, File.READ)
		var data = {}
		var text = file.get_as_text()
		current_lvl = parse_json(text)
		file.close()
		# generate new level if settings dont match
		if current_lvl["generator"] != game_settings["generator"].hash():
			generate_level(lvl_gen_sound, song_id)
	else:
		# otherwise we generate the level
		generate_level(lvl_gen_sound, song_id)


func generate_level(snd, song_id):
	var fileops = File.new()
	# generate level from sound
	current_lvl = LevelGenerator.generate_level(snd)
	current_lvl["generator"] = game_settings["generator"].hash()
	# save it
	fileops.open("user://level_cache/%s.arl" % song_id, File.WRITE)
	fileops.store_line(to_json(current_lvl))
	fileops.close()
	

func save_score(score, name):
	var fileops:File = File.new()
	var leaderboard
	if fileops.file_exists("user://local_leaderboard/%s.arl" % song_id):
		fileops.open("user://local_leaderboard/%s.arl" % song_id, File.READ)
		leaderboard = parse_json(fileops.get_as_text())
		fileops.close()
	else:
		leaderboard = {}
	
	leaderboard[floor(score)] = name
	
	# save the leaderboard
	fileops.open("user://local_leaderboard/%s.arl" % song_id, File.WRITE)
	fileops.store_line(to_json(leaderboard))
	fileops.close()
