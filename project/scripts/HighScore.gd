extends Control;

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
		# get array of keys (the actual scores) and sort it
		var keys = highscores.keys();
		keys.sort();
		# create leaderboard text
		var scoretext = "";
		for i in range(0,len(keys)):
			scoretext += "%s - %s\n" % [keys[len(keys)-1-i], highscores[keys[len(keys)-1-i]]];
		$Scores.text = scoretext;


func _on_RetryButton_pressed():
	get_tree().change_scene("res://scenes/Game.tscn");


func _on_MenuButton_pressed():
	get_tree().change_scene("res://scenes/Menus.tscn");
