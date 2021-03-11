extends Node

func _on_Play_pressed():
	$Titlescreen.hide()
	$SongSelect.show()
	$SongSelect/FileDialog.popup()
	#$SongSelect/FileDialog.rect_position.y = 0
	#get_node("/root/Root").new_scene("res://scenes/Game.tscn")
	#queue_free()
