extends Node

var scene;

# Called when the node enters the scene tree for the first time.
func _ready():
	# load level cache
	
	# start menu scene
	scene = add_child(load("res://scenes/Menus.tscn").instance())

func new_scene(s):
	remove_child(scene)
	scene = add_child(load(s).instance())
