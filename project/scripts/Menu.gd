extends Node;

func _on_Play_pressed():
	$Titlescreen/FileDialog.popup();
	$Titlescreen/FileDialog.rect_position.x = 0;
	$Titlescreen/FileDialog.rect_position.y = 0;


func _on_FileDialog_file_selected(path):
	Global.current_lvl_path = path;
	# open generator settings screen
	$Titlescreen/GeneratorSettings/MInterval_HSlider.value = Global.game_settings["generator"]["min_interval"];
	$Titlescreen/GeneratorSettings/Sensitivity_HSlider.value = Global.game_settings["generator"]["sensitivity"];
	$Titlescreen/GeneratorSettings/Balance_HSlider.value = Global.game_settings["generator"]["balance"];
	$Titlescreen/GeneratorSettings/TwoPlayerMode.pressed = Global.two_player_mode;
	$Titlescreen/GeneratorSettings.show();


func _on_Reset_pressed():
	$Titlescreen/GeneratorSettings/MInterval_HSlider.value = Global.game_settings["generator"]["min_interval"];
	$Titlescreen/GeneratorSettings/Sensitivity_HSlider.value = Global.game_settings["generator"]["sensitivity"];
	$Titlescreen/GeneratorSettings/Balance_HSlider.value = Global.game_settings["generator"]["balance"];


func _on_Start_pressed():
	# set two player mode
	Global.two_player_mode = $Titlescreen/GeneratorSettings/TwoPlayerMode.pressed;
	# set generator settings
	Global.game_settings["generator"]["min_interval"] = $Titlescreen/GeneratorSettings/MInterval_HSlider.value;
	Global.game_settings["generator"]["sensitivity"] = $Titlescreen/GeneratorSettings/Sensitivity_HSlider.value;
	Global.game_settings["generator"]["balance"] = $Titlescreen/GeneratorSettings/Balance_HSlider.value;
	Global.LevelGenerator.set_settings(Global.game_settings["generator"]["min_interval"], 0.76-Global.game_settings["generator"]["sensitivity"], Global.game_settings["generator"]["balance"]);
	# save generator settings
	Global.save_settings();
	# load level
	Global.load_level(Global.current_lvl_path);
	if Global.two_player_mode:
		get_tree().change_scene("res://scenes/Game2P.tscn");
	else:
		get_tree().change_scene("res://scenes/Game.tscn");


func _on_Options_pressed():
	$Titlescreen/Options.show();
	$Titlescreen/Options/PlayerName.text = Global.game_settings["defname"];
	$Titlescreen/Options/PlayerName2.text = Global.game_settings["defname2"];


func _on_Back_pressed():
	# close options window
	$Titlescreen/Options.hide();


func _on_Save_pressed():
	# change settings dict
	Global.game_settings["defname"] = $Titlescreen/Options/PlayerName.text;
	Global.game_settings["defname2"] = $Titlescreen/Options/PlayerName2.text;
	# save settings dict
	var fileops:File = File.new();
	fileops.open("user://game.acf", File.WRITE);
	fileops.store_line(to_json(Global.game_settings));
	fileops.close();


func _on_Exit_pressed():
	# exit the game
	get_tree().quit();
