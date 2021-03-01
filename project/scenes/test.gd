# This code is adapted from the official Godot Docs section on GDNative in C
# https://docs.godotengine.org/en/stable/tutorials/plugins/gdnative/gdnative-c-example.html

extends Control

# load the test library
onready var data = preload("res://bin/scripts/Test.gdns").new()
onready var FMODSound = preload("res://bin/scripts/FMODSound.gdns").new()

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func _on_Button_pressed():
	$FileDialog.popup()
	$FileDialog.rect_position = Vector2(0,0);


func _on_FileDialog_file_selected(path):
	$Label.text = str(FMODSound.create(path))
	$Label2.text = str(FMODSound.play())
