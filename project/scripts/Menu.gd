extends Node;

# play button, opens file select dialog
func _on_Play_pressed():
	$Titlescreen/FileDialog.popup();
	$Titlescreen/FileDialog.rect_position.x = 0;
	$Titlescreen/FileDialog.rect_position.y = 0;


# set current song path and open level generator settings menu
func _on_FileDialog_file_selected(path):
	Global.current_lvl_path = path;
	# open generator settings screen
	$Titlescreen/GeneratorSettings/DifficultySelector.selected = Global.game_settings["difficulty"];
	$Titlescreen/GeneratorSettings/MInterval_HSlider.value = Global.game_settings["generator"]["min_interval"];
	$Titlescreen/GeneratorSettings/Sensitivity_HSlider.value = Global.game_settings["generator"]["sensitivity"];
	$Titlescreen/GeneratorSettings/TwoPlayerMode.pressed = Global.two_player_mode;
	$Titlescreen/GeneratorSettings.show();


# reset level generator sliders to their previous position
func _on_Reset_pressed():
	$Titlescreen/GeneratorSettings/MInterval_HSlider.value = Global.game_settings["generator"]["min_interval"];
	$Titlescreen/GeneratorSettings/Sensitivity_HSlider.value = Global.game_settings["generator"]["sensitivity"];


# send level generator settings to level generator & load/generate the level, then play the game
func _on_Start_pressed():
	# set two player mode
	Global.two_player_mode = $Titlescreen/GeneratorSettings/TwoPlayerMode.pressed;
	# set generator settings
	Global.game_settings["generator"]["min_interval"] = $Titlescreen/GeneratorSettings/MInterval_HSlider.value;
	Global.game_settings["generator"]["sensitivity"] = $Titlescreen/GeneratorSettings/Sensitivity_HSlider.value;
	Global.LevelGenerator.set_settings(Global.game_settings["generator"]["min_interval"], 0.76-Global.game_settings["generator"]["sensitivity"]);
	# save generator settings
	Global.save_settings();
	# load level
	Global.load_level(Global.current_lvl_path);
	# open game scene for 1 or 2 players depending on the setting
	if Global.two_player_mode:
		get_tree().change_scene("res://scenes/Game2P.tscn");
	else:
		get_tree().change_scene("res://scenes/Game.tscn");


# Open options screen
func _on_Options_pressed():
	$Titlescreen/Options.show();
	$Titlescreen/Options/PlayerName.text = Global.game_settings["defname"];
	$Titlescreen/Options/PlayerName2.text = Global.game_settings["defname2"];


# Close options screen
func _on_Back_pressed():
	$Titlescreen/Options.hide();


# Save options
func _on_Save_pressed():
	# change settings dict
	Global.game_settings["defname"] = $Titlescreen/Options/PlayerName.text;
	Global.game_settings["defname2"] = $Titlescreen/Options/PlayerName2.text;
	# save settings dict
	var fileops:File = File.new();
	fileops.open("user://game.acf", File.WRITE);
	fileops.store_line(to_json(Global.game_settings));
	fileops.close();


# Exit the game
func _on_Exit_pressed():
	get_tree().quit();


# Change sliders to values for a difficulty preset
func _on_DifficultySelector_item_selected(index):
	# check if the selected difficulty is not "Custom"
	if index != 3:
		# set sliders to preset value
		$Titlescreen/GeneratorSettings/MInterval_HSlider.value = Global.DIFFICULTY_PRESETS[index*2];
		$Titlescreen/GeneratorSettings/Sensitivity_HSlider.value = Global.DIFFICULTY_PRESETS[index*2+1];
	else:
		# set sliders to player stored custom value
		$Titlescreen/GeneratorSettings/MInterval_HSlider.value = Global.game_settings["generator"]["min_interval"];
		$Titlescreen/GeneratorSettings/Sensitivity_HSlider.value = Global.game_settings["generator"]["sensitivity"];


# changed a value, custom difficulty now
func _on_MInterval_HSlider_value_changed(value):
	Global.game_settings["difficulty"] = 3;

# ditto
func _on_Sensitivity_HSlider_value_changed(value):
	Global.game_settings["difficulty"] = 3;
