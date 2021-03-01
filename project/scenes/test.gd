extends Control

onready var FMODSound = preload("res://bin/scripts/FMODSound.gdns").new()
onready var LevelGenerator = preload("res://bin/scripts/LevelGenerator.gdns").new()

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func _on_Button_pressed():
	$FileDialog.popup()
	$FileDialog.rect_position = Vector2(0,0);


func _on_FileDialog_file_selected(path):
	$Label.text = "FMOD Sound create result: " + str(FMODSound.create(path))
	var lvl = LevelGenerator.generate_level(FMODSound)
	FMODSound.play()
	$Label2.text = str(lvl)
