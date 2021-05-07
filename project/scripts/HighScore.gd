# Logic and control for the high score screen

extends Control;

# custom sort comparison function for sorting values in descending order
class Sorters:
	static func descending(a, b):
		if a > b: return true;

# Called when the node enters the scene tree for the first time.
func _ready():
	# set header text
	$Header.text = $Header.text % [Global.current_lvl["metadata"][1],Global.current_lvl["metadata"][0]];
	# open and read high score data
	var fileops = File.new();
	if fileops.file_exists("user://local_leaderboard/%s.arl" % Global.song_id):
		fileops.open("user://local_leaderboard/%s.arl" % Global.song_id, File.READ);
		var text = fileops.get_as_text();
		var highscores = parse_json(text);
		fileops.close();
		
		# get array of keys (the actual scores)
		var keys = highscores.keys();
		# convert to integers & sort
		for i in range(0,len(keys)):
			keys[i] = int(keys[i]);
		keys.sort_custom(Sorters, "descending");
		
		# create leaderboard text
		var scoretext = "";
		for i in range(0,len(keys)):
			scoretext += "%s - %s\n" % [keys[i], highscores[str(keys[i])]];
		$Scores.text = scoretext;


# retry this song
func _on_RetryButton_pressed():
	if Global.two_player_mode:
		get_tree().change_scene("res://scenes/Game2P.tscn");
	else:
		get_tree().change_scene("res://scenes/Game.tscn");


# return to the main menu
func _on_MenuButton_pressed():
	get_tree().change_scene("res://scenes/Menus.tscn");
