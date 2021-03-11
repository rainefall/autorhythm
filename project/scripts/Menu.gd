extends Node

func _on_Play_pressed():
	$Titlescreen.hide()
	$SongSelect.show()
	$SongSelect/FileDialog.popup()
	$SongSelect/FileDialog.rect_position.y = 0


func _on_FileDialog_file_selected(path):
	Global.load_level(path)
	get_tree().change_scene("res://scenes/Game.tscn")
