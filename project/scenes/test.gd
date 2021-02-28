# This code is adapted from the official Godot Docs section on GDNative in C
# https://docs.godotengine.org/en/stable/tutorials/plugins/gdnative/gdnative-c-example.html

extends Control

# load the test library
onready var data = preload("res://bin/scripts/Test.gdns").new()

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func _on_Button_pressed():
	$Label.text = data.get_data()
