extends Node

func _on_Play_pressed():
	$Titlescreen/FileDialog.popup()
	$Titlescreen/FileDialog.rect_position.x = 0
	$Titlescreen/FileDialog.rect_position.y = 0


func _on_FileDialog_file_selected(path):
	Global.load_level(path)
	get_tree().change_scene("res://scenes/Game.tscn")
